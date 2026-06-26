# 06 Storage / File System

## Goal

Study file descriptors, buffered I/O, page cache, `mmap` file I/O, `fsync`, and direct I/O.

## Core Questions

1. Why is file I/O usually mediated by the page cache?
2. What does `fsync` cost?
3. How does `mmap` file I/O differ from `read`/`write`?
4. When does direct I/O matter?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `read_write_buffered.c` | How does buffered I/O behave? | page cache |
| `mmap_file_io.c` | How does file mapping work? | mmap |
| `fsync_cost.c` | What does durability cost? | flush to storage |
| `direct_io.c` | What does bypassing page cache change? | direct I/O |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- file descriptor
- inode
- VFS
- page cache
- buffered I/O
- direct I/O
- fsync

## Connection to Modern Systems

Storage is not the main line of this repository, but page cache and mmap are important for virtual memory, checkpointing, logs, and shared file-backed memory.
