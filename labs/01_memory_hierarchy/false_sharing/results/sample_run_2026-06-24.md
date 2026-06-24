# False Sharing Sample Run

Date: 2026-06-24

Command:

```bash
make run-false-sharing
```

Output:

```text
iterations/thread: 200000000
cpu placement:     0, 1

            case      seconds   million writes/s        ns/write
   false sharing        0.279             1434.6           0.697
          padded        0.041             9682.0           0.103

speedup from padding: 6.75x
```

Observation:

The adjacent-counter layout is much slower because both threads repeatedly write different words in the same cache line. Padding separates the counters into different cache lines, reducing cache-coherence ownership transfers.
