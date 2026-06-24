# Analysis

## What Is the Question?

This experiment asks whether cache warmth is local to the CPU that touched the data, and whether moving execution to another CPU changes pointer-chasing latency.

## What Is the OS/System Concept?

The key ideas are CPU affinity, cache locality, shared L3 behavior, and NUMA-style locality. Even when a machine has one NUMA node, not every CPU necessarily has identical cache locality for recently touched data.

## How Is the Experiment Designed?

The benchmark creates a random pointer-chasing cycle. For each case, it warms the cycle while pinned to one CPU and then measures dependent-load latency while pinned to another CPU.

The default comparison is:

- same CPU: warm and measure on the same CPU
- near CPU: measure on the second allowed CPU
- far CPU: measure on the last allowed CPU

## How to Reproduce It?

```bash
make
make run
```

To choose CPUs manually:

```bash
./build/numa_or_l3_cluster 16777216 10000000 0 1 15
```

## What Is the Result?

The current sample run is:

```text
allowed CPUs:       32
working set:        16 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0          13.846            1.00
    near_cpu          0            1          12.625            0.91
     far_cpu          0           31          13.535            0.98
```

This result does not show a clear remote-cache penalty. `near_cpu` is slightly faster than `same_cpu`, and `far_cpu` is close to `same_cpu`.

## Why Is This Experiment Not Complete?

The labels `near_cpu` and `far_cpu` are only default guesses. The program currently chooses the first, second, and last allowed CPUs. Those logical CPU numbers do not necessarily correspond to hardware distance.

Several things can make the result look flat:

- CPU 0, CPU 1, and CPU 31 may share the same L3 cache or NUMA node.
- CPU 0 and CPU 1 may have a special relationship such as SMT sibling placement.
- A 16 MiB working set may fit inside shared L3 on this machine.
- The benchmark controls CPU affinity, but not physical page placement.
- CPU frequency scaling, interrupts, and scheduler noise can be larger than the topology effect.
- Timing alone cannot tell whether the cause is L1/L2 locality, shared L3, DRAM, or CPU frequency behavior.

So the current benchmark is useful as a first probe, but it is not enough to claim a complete NUMA or L3-cluster analysis.

## Further Experiments

The next step is to connect timing with topology. In this folder, that analysis is now supported by:

```text
scripts/collect_topology.sh
scripts/run_topology_cases.sh
scripts/run_working_set_sweep.sh
scripts/run_repeated_cases.sh
```

For the current machine, `lscpu -e=CPU,CORE,SOCKET,NODE,CACHE` shows:

- CPU 0 and CPU 16 share core 0 and L3 0, so they are SMT siblings.
- CPU 0 and CPU 1 are different cores but share L3 0.
- CPU 0 and CPU 8 are different cores and different L3 clusters.

That gives more meaningful manual placements than the earlier first/second/last CPU defaults.

The follow-up results are recorded in:

- [topology_summary_2026-06-24.md](results/topology_summary_2026-06-24.md)
- [topology_cases_2026-06-24.md](results/topology_cases_2026-06-24.md)
- [working_set_sweep_2026-06-24.md](results/working_set_sweep_2026-06-24.md)
- [repeated_cases_2026-06-24.md](results/repeated_cases_2026-06-24.md)

Current conclusion:

The follow-up runs do not show a stable, monotonic penalty for crossing from L3 instance 0 to L3 instance 1. The evidence is therefore negative or inconclusive rather than a clean demonstration of remote-cache slowdown.

The repeated 16 MiB run confirms that the medians are close:

```text
same CPU:  13.113 ns/access
same L3:   12.619 ns/access
other L3:  12.979 ns/access
```

The likely explanation is that this machine has one NUMA node, two L3 instances, dynamic CPU frequency behavior, and a workload whose bottleneck changes with working-set size. At 64 MiB, the benchmark likely spills beyond one L3 instance and becomes more DRAM/noise dominated. For smaller sizes, the measured differences are only a few percent and are comparable to run-to-run noise.

1. Map CPU topology with `lscpu -e=CPU,CORE,SOCKET,NODE,CACHE` and sysfs `shared_cpu_list`.
2. Compare same CPU, SMT sibling, different physical core with same L3, and different L3 cluster if available.
3. Sweep working-set sizes from L2-sized to larger-than-L3.
4. Use `numactl` on multi-node machines to separate CPU placement from memory placement.
5. Run `perf stat` to collect cycles, instructions, cache references, and cache misses.
6. Repeat each placement and report min, median, and max.

See [future_experiments.md](future_experiments.md) for a concrete checklist.

## What Does It Prove or Not Prove?

It can show whether moving between CPUs changes access latency after warm-up. That is evidence of local/remote cache effects or topology effects when the result is stable and aligned with topology data.

It does not by itself prove the exact cause. To distinguish SMT sibling effects, shared L3 clusters, and NUMA nodes, compare with CPU topology data from `lscpu`, sysfs, `numactl`, repeated runs, and perf counters.
