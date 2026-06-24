# Analysis

## What Is the Question?

This experiment asks why two threads can interfere with each other even when they write different variables.

## What Is the OS/System Concept?

The core concept is cache coherence. CPU caches keep memory coherent at cache-line granularity. If two cores repeatedly write different words in the same cache line, the line keeps moving between cores.

## How Is the Experiment Designed?

The benchmark compares two layouts:

- `shared_counters_t`: two adjacent counters likely sharing one cache line
- `padded_counters_t`: each counter separated into its own 64-byte cache line

Two threads increment different counters for the same number of iterations. The program reports throughput and `ns/write` for both layouts.

## How to Reproduce It?

```bash
make
make run
```

To control CPU placement:

```bash
./build/false_sharing 200000000 0 1
```

## What Is the Result?

Record machine-specific output under `results/` or repository-level `results/raw/`.

## What Does It Prove or Not Prove?

It shows that data layout matters for multi-threaded performance. Two logically independent variables can still cause heavy coherence traffic if they share one cache line.

It does not fully isolate all scheduler or topology effects. Results can change depending on whether the chosen CPUs are SMT siblings, different physical cores, or different cache clusters.
