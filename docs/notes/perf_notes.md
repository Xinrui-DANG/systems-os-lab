# perf Notes

Common commands:

```bash
perf stat ./program
perf stat -e cache-misses,cache-references ./program
perf stat -e page-faults ./program
```
