# Prefetch Effect Sample Run

Date: 2026-06-24

Command:

```bash
make run-prefetch-effect
```

Output:

```text
working set:        256 MiB
passes:             8
prefetch distance:  64 elements (512 bytes)

              case       ns/access           MiB/s   vs sequential
        sequential           0.237         32173.3            1.00
 software_prefetch           0.344         22204.7            1.45
      random_chase         101.448            75.2          427.81
```

Observation:

The plain sequential scan is faster than the explicit software-prefetch version in this run. That suggests the hardware prefetcher already handles this predictable stream well, while the extra prefetch instructions add overhead. Random dependent pointer chasing is hundreds of times slower because the next address is not known early enough to prefetch.
