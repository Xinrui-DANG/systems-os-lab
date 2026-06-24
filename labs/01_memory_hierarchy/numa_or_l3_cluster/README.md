# NUMA or L3 Cluster

This experiment explores local and remote cache effects by warming data on one CPU and measuring pointer-chasing latency on another CPU.

## Question

Does memory-access latency change when data is warmed on one CPU and then consumed on another CPU?

## Concept

- local vs remote cache effects
- shared L3 cache
- L3 cluster or cache slice effects
- CPU affinity
- NUMA-style thinking

## Design

The benchmark builds a random pointer-chasing cycle over a 16 MiB working set by default.

For each case, it:

1. Pins the process to a warm-up CPU.
2. Walks the pointer cycle to warm cache state from that CPU.
3. Pins the process to a measurement CPU.
4. Times dependent pointer chasing.

Default cases:

- `same_cpu`: warm and measure on the same CPU
- `near_cpu`: warm on the first allowed CPU, measure on the second allowed CPU
- `far_cpu`: warm on the first allowed CPU, measure on the last allowed CPU

This does not assume the machine has multiple NUMA nodes. On a single-node system, it can still reveal differences between private caches, shared L3, or CPU clusters.

## Reproduce

From the repository root:

```bash
make numa-or-l3-cluster
make run-numa-or-l3-cluster
```

Or from this directory:

```bash
make
make run
```

Optional arguments:

```bash
./build/numa_or_l3_cluster <bytes> <timed_steps> <warm_cpu> <near_cpu> <far_cpu>
```

Example:

```bash
./build/numa_or_l3_cluster 16777216 10000000 0 1 15
```

## Follow-Up Analysis Scripts

This folder also contains scripts for turning the first probe into a topology-aware analysis:

```bash
./scripts/collect_topology.sh results
./scripts/run_topology_cases.sh
./scripts/run_working_set_sweep.sh
./scripts/run_repeated_cases.sh
```

The current machine used in the sample analysis has two visible L3 clusters in `lscpu` output:

- L3 `0`: CPUs 0-7 and 16-23
- L3 `1`: CPUs 8-15 and 24-31

That makes these useful manual placements:

- CPU 0 -> CPU 16: SMT sibling
- CPU 0 -> CPU 1: different core, same L3
- CPU 0 -> CPU 8: different core, different L3 cluster

## Results

The output reports `ns/access` and slowdown relative to `same_cpu`.

## Interpretation

If `near_cpu` or `far_cpu` is slower than `same_cpu`, the data was less local to the measuring CPU. Causes can include private L1/L2 cache misses, shared vs remote L3 behavior, CPU cluster boundaries, or NUMA placement.

This is an exploratory topology benchmark, not a complete proof by itself. The default CPU choices are only guesses based on the process's allowed CPU set. Interpret results together with `lscpu`, `/sys/devices/system/cpu`, `numactl`, perf counters, and repeated runs.

## Limitations

This lab is incomplete if used alone because:

- logical CPU numbers do not directly encode hardware distance
- the benchmark controls CPU affinity but not NUMA page placement
- one working-set size cannot reveal every cache boundary
- scheduler noise and frequency scaling can hide small differences
- timing alone cannot identify the exact source of a latency difference

See [future_experiments.md](future_experiments.md) for the follow-up experiments needed to turn this probe into a stronger topology analysis.
