# Cache Line Effect Sample Run

Date: 2026-06-24

Command:

```bash
make run-cache-line-effect
```

Output:

```text
    Stride       ns/access   line utilization        accesses
      1 B           0.233            100.00%       268435456
      2 B           0.244             50.00%       134217728
      4 B           0.258             25.00%        67108864
      8 B           0.266             12.50%        33554432
     16 B           0.450              6.25%        16777216
     32 B           0.902              3.12%        16777216
     64 B           1.955              1.56%        12582912
    128 B           4.968              1.56%        10485760
    256 B           5.300              1.56%        10485760
    512 B           5.447              1.56%        10485760
   1024 B           5.776              1.56%        10223616
   4096 B           6.278              1.56%        10027008
```

Observation:

Small strides use more bytes from each fetched cache line. At stride 64 and above, each access uses only one byte from a 64-byte cache line, so useful cache-line utilization drops to 1.56%. The measured access cost rises as the stride wastes more of each fetched line.
