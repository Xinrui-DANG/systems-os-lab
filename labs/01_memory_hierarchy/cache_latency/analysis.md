# Analysis

## What Is the Question?

How does random dependent memory access latency change as the working set grows?

## What Is the OS/System Concept?

This experiment connects user-space memory access to the CPU cache hierarchy and DRAM. It is also a useful baseline before studying TLB behavior and page-table walks.

## How Is the Experiment Designed?

The program creates a random cycle of indexes and repeatedly follows it. Because every access depends on the previous result, the CPU cannot easily overlap or prefetch the next access.

## How to Reproduce It?

```bash
make
make run
```

For more stable results, pin it to one CPU:

```bash
taskset -c 2 ./build/cache_latency
```

## What Is the Result?

Record current machine-specific results under `results/`.

## What Does It Prove or Not Prove?

It shows the latency trend across working-set sizes on the current machine. It does not by itself identify exact cache sizes or all microarchitectural causes; compare with `lscpu`, sysfs cache data, and repeated runs.
