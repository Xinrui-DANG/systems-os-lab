# Analysis

## What Is the Question?

This experiment asks how prefetching changes memory-access cost, and why some patterns are easier to prefetch than others.

## What Is the OS/System Concept?

The core concept is latency hiding. CPUs try to fetch data before the program needs it. Hardware prefetchers infer patterns automatically, while software prefetch instructions let the program explicitly request future cache lines.

## How Is the Experiment Designed?

The benchmark compares three access patterns:

- sequential scan: predictable, hardware-prefetch friendly
- sequential scan with `__builtin_prefetch`: explicit software prefetch
- random pointer chasing: next address depends on the current load

The random pointer-chasing case is included to show the limitation of prefetching: if the address is not known early enough, there is little to prefetch.

## How to Reproduce It?

```bash
make
make run
```

To vary the working set, number of passes, or software prefetch distance:

```bash
./build/prefetch_effect 268435456 8 64
```

## What Is the Result?

Record machine-specific output under `results/` or repository-level `results/raw/`.

## What Does It Prove or Not Prove?

It shows that predictable access streams are much easier for the CPU to optimize. It also shows that explicit software prefetch is not automatically faster than normal sequential access, because hardware prefetching may already be effective.

It does not fully isolate every hardware prefetcher or memory-subsystem detail. Exact behavior depends on CPU microarchitecture, compiler, cache hierarchy, and memory bandwidth.
