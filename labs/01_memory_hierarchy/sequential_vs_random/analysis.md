# Analysis

## What Is the Question?

This experiment asks how much access pattern matters for memory performance.

## What Is the OS/System Concept?

The core concept is hardware prefetching. Sequential access exposes a predictable stream of addresses, while random dependent access does not.

## How Is the Experiment Designed?

For each working-set size, the benchmark allocates one sequential data array and one random pointer-chasing cycle.

The sequential loop repeatedly reads adjacent elements. The random loop follows a shuffled cycle where each next address depends on the previous loaded value.

## How to Reproduce It?

```bash
make
make run
```

For a stable CPU placement:

```bash
taskset -c 2 ./build/sequential_vs_random
```

## What Is the Result?

Record machine-specific output under `results/` or repository-level `results/raw/`.

## What Does It Prove or Not Prove?

It shows that memory latency is not only about physical DRAM speed. Access pattern changes how much the CPU can hide latency through prefetching, cache-line reuse, and memory-level parallelism.

It does not isolate one single hardware mechanism. For exact attribution, compare with performance counters such as cache misses and prefetch-related events when available.
