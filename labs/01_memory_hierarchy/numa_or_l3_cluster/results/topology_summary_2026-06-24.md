# Topology Summary

Date: 2026-06-24

Command:

```bash
./scripts/collect_topology.sh results
lscpu -e=CPU,CORE,SOCKET,NODE,CACHE
```

Relevant topology:

```text
CPU CORE SOCKET NODE L1d:L1i:L2:L3
  0    0      0    0 0:0:0:0
  1    1      0    0 1:1:1:0
  8    8      0    0 8:8:8:1
 16    0      0    0 0:0:0:0
```

Interpretation:

- CPU 0 and CPU 16 are SMT siblings on core 0.
- CPU 0 and CPU 1 are different physical cores but share L3 instance 0.
- CPU 0 and CPU 8 are different physical cores and different L3 instances.
- The machine has one NUMA node, so this is an L3-cluster experiment rather than a true remote-NUMA-memory experiment.

Relevant `lscpu` summary:

```text
Model name:        AMD Ryzen 9 8945HX with Radeon Graphics
CPU(s):            32
Thread(s) per core: 2
Core(s) per socket: 16
Socket(s):         1
L2 cache:          16 MiB (16 instances)
L3 cache:          64 MiB (2 instances)
NUMA node(s):      1
NUMA node0 CPU(s): 0-31
```

Useful placements for this lab:

| Relationship | Warm CPU | Measure CPU |
|---|---:|---:|
| same logical CPU | 0 | 0 |
| SMT sibling | 0 | 16 |
| different core, same L3 | 0 | 1 |
| different core, different L3 | 0 | 8 |
