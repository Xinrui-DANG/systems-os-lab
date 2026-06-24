# False Sharing

This experiment measures the cost of cache-coherence traffic caused by false sharing.

## Question

Why can two threads writing different variables still slow each other down?

## Concept

- false sharing
- cache coherence
- cache line ownership
- multi-threaded writes
- padding and alignment

## Design

The benchmark runs two worker threads. Each thread repeatedly increments its own counter.

False-sharing case:

```c
typedef struct {
    volatile uint64_t a;
    volatile uint64_t b;
} shared_counters_t;
```

The two counters are adjacent and normally live in the same 64-byte cache line. Even though the threads write different variables, the cache-coherence protocol transfers ownership of the whole cache line between cores.

Padded case:

```c
typedef struct {
    volatile uint64_t value;
    char padding[64 - sizeof(uint64_t)];
} padded_counter_t;
```

Each counter occupies its own cache line, so the threads no longer fight over the same line.

## Reproduce

From the repository root:

```bash
make false-sharing
make run-false-sharing
```

Or from this directory:

```bash
make
make run
```

You can also choose the iteration count and CPU placement:

```bash
./build/false_sharing 200000000 0 1
```

## Results

The output reports runtime, throughput, ns/write, and the speedup from padding.

## Interpretation

If false sharing is present, the adjacent-counter case should be slower than the padded-counter case. Padding helps because cache coherence operates at cache-line granularity, not variable granularity.
