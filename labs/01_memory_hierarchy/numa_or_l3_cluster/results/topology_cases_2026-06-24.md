# Topology Cases

Date: 2026-06-24

Command:

```bash
./scripts/run_topology_cases.sh 16777216 10000000 0 16 1 8
```

Placement meaning:

- CPU 0 -> CPU 16: SMT sibling
- CPU 0 -> CPU 1: different physical core, same L3
- CPU 0 -> CPU 8: different physical core, different L3

Output:

```text
# same CPU, SMT sibling, same-L3 core
allowed CPUs:       32
working set:        16 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0          14.040            1.00
    near_cpu          0           16          12.417            0.88
     far_cpu          0            1          13.139            0.94

# same CPU, same-L3 core, other-L3 core
allowed CPUs:       32
working set:        16 MiB
timed accesses:     10000000

        case   warm cpu  measure cpu       ns/access     vs same_cpu
    same_cpu          0            0          12.479            1.00
    near_cpu          0            1          13.891            1.11
     far_cpu          0            8          12.998            1.04
```

Analysis:

This result does not support a simple rule like "different L3 is always slower." In the second run, CPU 0 -> CPU 1, which stays within L3 instance 0, is slower than CPU 0 -> CPU 8, which crosses to L3 instance 1.

Likely reasons:

- The benchmark is sensitive to CPU frequency and transient system noise.
- The 16 MiB working set is smaller than the full 32 MiB per L3 instance implied by 64 MiB across 2 L3 instances.
- Pointer chasing measures dependent-load latency, but not every access is guaranteed to hit the warmed cache level after CPU migration.
- This machine has only one NUMA node, so remote DRAM placement is not being tested.

Conclusion:

The topology-aware run is useful, but still not conclusive. It motivates a working-set sweep and repeated-run statistics.
