# NUMA or L3 Cluster Sample Run

Date: 2026-06-24

Command:

```bash
make run-numa-or-l3-cluster
```

Output:

```text
allowed CPUs:       32
working set:        16 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0          13.846            1.00
    near_cpu          0            1          12.625            0.91
     far_cpu          0           31          13.535            0.98
```

Observation:

This run does not show a strong remote-cache penalty between CPU 0, CPU 1, and CPU 31. That suggests the tested placement may share enough cache hierarchy for this 16 MiB working set, or that scheduler/frequency noise is larger than the topology effect in this run. Interpret this experiment together with `lscpu` and repeated manual CPU placements.

Reasoning:

This sample should not be interpreted as proof that there is no local/remote cache effect. The chosen logical CPUs may not cross a real L3 or NUMA boundary, and the benchmark does not yet control physical page placement. A stronger follow-up needs CPU topology mapping, working-set sweeps, manual CPU placement, NUMA memory binding when available, perf counters, and repeated runs.
