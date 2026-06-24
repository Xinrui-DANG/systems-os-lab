# Follow-Up Experiments

This lab is intentionally exploratory. The first benchmark can show whether moving between CPUs changes latency, but it cannot alone explain why. The follow-up work connects measurements with CPU topology.

The current folder now contains scripts for the next analysis stage:

```text
scripts/collect_topology.sh
scripts/run_topology_cases.sh
scripts/run_working_set_sweep.sh
scripts/run_repeated_cases.sh
```

## 1. CPU Topology Map

Question:

Which logical CPUs are SMT siblings, which cores share L2, and which CPUs share the same L3 cache?

Commands:

```bash
./scripts/collect_topology.sh results
```

Reason:

The current default choices use the first, second, and last allowed CPUs. Those IDs may not mean near or far in hardware topology. CPU 0 and CPU 31 can still share an L3 cache or NUMA node, depending on the machine.

## 2. SMT Sibling vs Different Physical Core

Question:

Is measuring on an SMT sibling different from measuring on another physical core?

Experiment:

Use topology data to choose:

- CPU A: warm and measure on the same logical CPU
- CPU B: SMT sibling of CPU A
- CPU C: different physical core, same L3

Example:

```bash
./scripts/run_topology_cases.sh 16777216 10000000 <cpu_a> <smt_sibling> <same_l3_core> <other_l3_core>
```

Reason:

SMT siblings share core-private resources more closely than different cores. If the working set is affected by L1/L2 locality, SMT sibling behavior can differ from same-L3 different-core behavior.

## 3. Same L3 vs Different L3 Cluster

Question:

Does crossing an L3 cluster boundary increase dependent-load latency?

Experiment:

Choose CPUs from:

- same core or same L3 cluster
- different L3 cluster, if the CPU has multiple L3 instances

Reason:

Some CPUs expose multiple L3 slices or clusters. A random pointer-chasing working set warmed from one cluster may not be equally local to another cluster.

## 4. Working-Set Sweep

Question:

At which working-set size do locality differences appear?

Experiment:

Run the same CPU placements with different working-set sizes:

```bash
./scripts/run_working_set_sweep.sh 10000000 <warm> <same_l3_core> <other_l3_core>
```

Reason:

A 16 MiB working set may fit in shared L3 on some machines. Smaller sets may mostly test private cache behavior, while larger sets may spill to DRAM and hide L3-locality differences.

## 5. NUMA First-Touch Placement

Question:

If the machine has multiple NUMA nodes, does first-touch memory placement change latency?

Commands:

```bash
numactl --hardware
numactl --cpunodebind=0 --membind=0 ./build/numa_or_l3_cluster
numactl --cpunodebind=1 --membind=0 ./build/numa_or_l3_cluster
```

Reason:

Linux normally places physical pages on the NUMA node that first touches them. To test true remote DRAM effects, CPU placement and memory placement must be controlled separately.

## 6. perf Counter Validation

Question:

Do hardware counters support the latency interpretation?

Commands:

```bash
perf stat -e cycles,instructions,cache-references,cache-misses ./build/numa_or_l3_cluster
```

Reason:

Timing alone shows a symptom. Cache miss counters, cycles, and instructions help separate topology effects from noise, frequency changes, and measurement overhead.

## 7. Repeated Runs and Statistics

Question:

Is the observed difference stable?

Experiment:

Run each placement many times and report min, median, and max latency.

Command:

```bash
./scripts/run_repeated_cases.sh 7 16777216 10000000 <warm> <same_l3_core> <other_l3_core>
```

Reason:

Scheduler activity, CPU frequency scaling, interrupts, and thermal behavior can hide small locality effects. One sample run is not enough to claim a topology result.
