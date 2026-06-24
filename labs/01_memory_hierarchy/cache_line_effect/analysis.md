# Analysis

## What Is the Question?

This experiment asks how much cache-line utilization matters when a program reads memory with different strides.

## What Is the OS/System Concept?

The core concept is spatial locality. CPUs move memory into cache in fixed-size cache lines, commonly 64 bytes. If a program reads neighboring bytes, one cache-line fetch serves many useful accesses. If it jumps by large strides, most bytes in each fetched line are unused.

## How Is the Experiment Designed?

The benchmark allocates a 256 MiB byte array and measures loops that read one byte every `stride` bytes.

Small strides reuse more of each fetched cache line. Large strides touch fewer useful bytes per line and eventually behave like one useful byte per cache line or per page-sized gap.

## How to Reproduce It?

```bash
make
make run
```

For stable CPU placement:

```bash
taskset -c 2 ./build/cache_line_effect
```

## What Is the Result?

Record machine-specific output under `results/` or repository-level `results/raw/`.

## What Does It Prove or Not Prove?

It shows that memory performance depends not only on whether data is cached, but also on how much useful data each cache-line fetch provides.

It does not isolate cache-line effects perfectly from hardware prefetching, TLB behavior, or DRAM row-buffer effects. Those mechanisms can also influence stride-based access.
