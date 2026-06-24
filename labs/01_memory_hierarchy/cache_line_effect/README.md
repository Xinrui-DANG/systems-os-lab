# Cache Line Effect

This experiment measures how stride changes spatial locality and cache-line utilization.

## Question

What happens when a program uses only part of each fetched cache line?

## Concept

- spatial locality
- cache line
- cache-line utilization
- stride access
- memory bandwidth waste

## Design

The benchmark allocates a 256 MiB byte array and reads it with different byte strides:

```text
1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 4096
```

For a typical 64-byte cache line:

- stride 1 uses all bytes in a cache line
- stride 8 uses 8 bytes from each cache line
- stride 64 uses 1 byte from each cache line
- stride 4096 uses 1 byte after each page-sized gap

## Reproduce

From the repository root:

```bash
make cache-line-effect
make run-cache-line-effect
```

Or from this directory:

```bash
make
make run
```

For a more stable run:

```bash
taskset -c 2 ./build/cache_line_effect
```

## Results

The output reports:

- `Stride`: distance between two loaded bytes
- `ns/access`: average load time for that stride
- `line utilization`: estimated useful bytes per 64-byte cache line
- `accesses`: number of timed loads

## Interpretation

Larger strides reduce spatial locality. Even when hardware prefetching can detect a regular stride, the program uses fewer bytes from each cache line, so more fetched memory bandwidth is wasted.
