# Working-Set Sweep

Date: 2026-06-24

Command:

```bash
./scripts/run_working_set_sweep.sh 10000000 0 1 8
```

Placement meaning:

- same CPU: CPU 0 -> CPU 0
- same L3: CPU 0 -> CPU 1
- other L3: CPU 0 -> CPU 8

Output:

```text
# working set: 1048576 bytes
allowed CPUs:       32
working set:        1 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0           4.916            1.00
    near_cpu          0            1           4.848            0.99
     far_cpu          0            8           5.143            1.05

# working set: 4194304 bytes
allowed CPUs:       32
working set:        4 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0           9.318            1.00
    near_cpu          0            1           9.320            1.00
     far_cpu          0            8           9.493            1.02

# working set: 16777216 bytes
allowed CPUs:       32
working set:        16 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0          13.187            1.00
    near_cpu          0            1          13.795            1.05
     far_cpu          0            8          13.142            1.00

# working set: 67108864 bytes
allowed CPUs:       32
working set:        64 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0          76.451            1.00
    near_cpu          0            1          75.879            0.99
     far_cpu          0            8          66.827            0.87
```

Analysis:

The sweep does not show a stable monotonic penalty for crossing from L3 instance 0 to L3 instance 1.

Observed pattern:

- At 1 MiB, other-L3 is about 5% slower.
- At 4 MiB, other-L3 is about 2% slower.
- At 16 MiB, same-L3 is about 5% slower while other-L3 is roughly equal to same CPU.
- At 64 MiB, other-L3 is faster in this run.

Reasoning:

The 64 MiB case likely spills beyond a single L3 instance, so the benchmark becomes dominated by DRAM behavior, memory-level effects, and frequency/noise rather than pure L3 locality. The smaller cases show only weak differences, which means a stronger conclusion would require repeated runs and statistics.

Conclusion:

For this machine and this single-run sweep, the experiment does not prove a clear L3-cluster penalty. The next improvement should be repeated trials with min/median/max and, if permissions allow, perf counters.
