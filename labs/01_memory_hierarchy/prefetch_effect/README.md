# Prefetch Effect

This experiment compares hardware-friendly sequential access, explicit software prefetching, and random dependent pointer chasing.

## Question

When does prefetching help, and when is memory access too unpredictable for prefetching to hide latency?

## Concept

- hardware prefetching
- software prefetching
- memory latency hiding
- predictable address streams
- random dependent loads

## Design

The benchmark uses a 256 MiB working set by default and measures three cases.

Sequential access:

```c
sum += data[i];
```

Software prefetch:

```c
__builtin_prefetch(&data[i + distance], 0, 1);
sum += data[i];
```

Random pointer chasing:

```c
idx = next[idx];
```

Sequential access gives the hardware prefetcher a clear address stream. Software prefetch explicitly requests a future element. Random pointer chasing is the contrast case: the next address is not known until the current load completes.

## Reproduce

From the repository root:

```bash
make prefetch-effect
make run-prefetch-effect
```

Or from this directory:

```bash
make
make run
```

Optional arguments:

```bash
./build/prefetch_effect <bytes> <passes> <prefetch_distance_elements>
```

Example:

```bash
./build/prefetch_effect 268435456 8 64
```

## Results

The output reports `ns/access`, throughput in `MiB/s`, and slowdown relative to sequential access.

## Interpretation

Hardware prefetching usually handles simple sequential access well. Software prefetch may help less than expected for already predictable streams because hardware prefetching is already active. Random dependent loads remain much slower because there is no useful future address to prefetch.
