# Sequential vs Random Access Sample Run

Date: 2026-06-24

Command:

```bash
make run-sequential-vs-random
```

Output:

```text
 Working set   seq ns/access   rnd ns/access      rnd/seq
        4 KiB           0.385           0.965         2.50
        8 KiB           0.381           0.957         2.52
       16 KiB           0.381           0.955         2.50
       32 KiB           0.383           0.962         2.51
       64 KiB           0.380           1.965         5.17
      128 KiB           0.381           2.479         6.51
      256 KiB           0.380           2.640         6.94
      512 KiB           0.384           3.425         8.93
     1024 KiB           0.385           4.894        12.72
     2048 KiB           0.380           7.682        20.20
     4096 KiB           0.384           9.349        24.36
     8192 KiB           0.392          10.403        26.53
    16384 KiB           0.413          13.199        31.94
    32768 KiB           0.434          40.031        92.18
    65536 KiB           0.426          73.201       171.97
   131072 KiB           0.434          90.404       208.25
   262144 KiB           0.437          99.697       228.36
```

Observation:

Sequential access stays below 0.5 ns/access in this run, while random dependent pointer chasing rises to about 100 ns/access for large working sets. This demonstrates how predictable access patterns let the CPU hide memory latency through prefetching and overlap.
