# Repeated Cases

Date: 2026-06-24

Command:

```bash
./scripts/run_repeated_cases.sh 7 16777216 10000000 0 1 8
```

Placement meaning:

- same CPU: CPU 0 -> CPU 0
- same L3: CPU 0 -> CPU 1
- other L3: CPU 0 -> CPU 8

Summary:

```text
repeats:        7
working set:    16 MiB
timed accesses: 10000000
placement:      warm=0 same_l3=1 other_l3=8

      case       min_ns    median_ns       max_ns
  same_cpu       11.963       13.113       13.876
   same_l3       12.032       12.619       15.222
  other_l3       11.892       12.979       13.238
```

Raw runs:

```text
1 same_cpu 13.124
1 same_l3 12.446
1 other_l3 13.183
2 same_cpu 13.113
2 same_l3 12.573
2 other_l3 12.264
3 same_cpu 13.452
3 same_l3 15.222
3 other_l3 13.021
4 same_cpu 12.482
4 same_l3 12.751
4 other_l3 11.963
5 same_cpu 13.027
5 same_l3 12.619
5 other_l3 13.238
6 same_cpu 11.963
6 same_l3 12.032
6 other_l3 11.892
7 same_cpu 13.876
7 same_l3 12.800
7 other_l3 12.979
```

Analysis:

The medians are close:

- same CPU: 13.113 ns/access
- same L3: 12.619 ns/access
- other L3: 12.979 ns/access

This repeated run still does not show a stable penalty for crossing from L3 instance 0 to L3 instance 1. The variation inside each case is comparable to the difference between cases.

Conclusion:

For this CPU placement and 16 MiB working set, the correct conclusion is not "remote L3 is slower." The better conclusion is: this benchmark did not isolate a strong L3-cluster effect on this machine. Further work would need tighter control over frequency, interrupts, memory placement, and hardware counters.
