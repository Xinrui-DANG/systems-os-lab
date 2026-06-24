# Sequential vs Random Access

This experiment compares sequential array access with random dependent pointer chasing.

## Question

Why is sequential memory access much faster than random pointer chasing, especially when the working set is larger than cache?

## Concept

- hardware prefetching
- spatial locality
- cache-line utilization
- memory-level parallelism
- dependent load latency

## Design

The benchmark measures two access patterns over the same working-set sizes.

Sequential access:

```c
sum += data[idx];
idx = (idx + 1) & mask;
```

Random pointer chasing:

```c
idx = next[idx];
```

Sequential access creates a predictable address stream. The CPU can prefetch future cache lines and overlap memory requests. Random pointer chasing hides the next address until the current load finishes, so it exposes much more of the real latency.

## Reproduce

From the repository root:

```bash
make sequential-vs-random
make run-sequential-vs-random
```

Or from this directory:

```bash
make
make run
```

For a more stable run, pin the benchmark to one CPU:

```bash
taskset -c 2 ./build/sequential_vs_random
```

## Results

The output reports:

- `seq ns/access`: average time per sequential load
- `rnd ns/access`: average time per random dependent load
- `rnd/seq`: how many times slower random chasing is

## Interpretation

If hardware prefetching and spatial locality are effective, sequential access should stay much cheaper than random pointer chasing. The gap usually grows when the working set no longer fits in cache.
