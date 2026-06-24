# Cache Latency

This experiment measures random dependent-load latency for increasing working-set sizes.

## Question

How does access latency change when the working set grows beyond L1, L2, L3, and finally reaches DRAM?

## Concept

- cache hierarchy
- dependent load latency
- pointer chasing
- working set
- cache boundary
- DRAM latency

## Design

The benchmark builds a shuffled linked cycle over an array of `size_t` indexes. Each load depends on the previous load:

```c
idx = next[idx];
```

This pointer-chasing pattern limits hardware prefetching and exposes approximate load latency at each working-set size.

## Reproduce

From the repository root:

```bash
make
make run
```

Or from this directory:

```bash
make
make run
```

To pin execution to one CPU:

```bash
taskset -c 2 ./build/cache_latency
```

## Results

Put raw outputs in `results/` or in the repository-level `results/raw/` directory.

## Interpretation

Latency should stay low while the working set fits in smaller caches, then rise at cache-capacity boundaries and rise again when accesses spill to DRAM.
