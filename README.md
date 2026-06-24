# Systems OS Lab

A structured operating-system and computer-architecture lab repository.

一个系统化的操作系统与计算机体系结构实验仓库。

## Goal

This repository follows the path from user programs to the kernel and hardware:

```text
Program
  -> Virtual Address
  -> TLB / Page Table
  -> Physical Address
  -> L1 / L2 / L3 Cache
  -> DRAM
  -> Storage / File System
  -> Process / Thread / Scheduler
  -> IPC / Synchronization
  -> Kernel / Driver / Mini OS
  -> Virtualization / Isolation
```

## Roadmap

1. Memory hierarchy
2. Virtual memory
3. Process and thread
4. Scheduler and CPU affinity
5. Synchronization
6. IPC
7. File system and I/O
8. Signal, timer, and interrupt-like behavior
9. Kernel module
10. Mini Unix kernel
11. RTOS experiments
12. Virtualization and isolation

See [ROADMAP.md](ROADMAP.md) for the detailed learning plan.

## Current Progress

| Module | Status | Path |
|---|---:|---|
| Cache latency pointer chasing | Done | `labs/01_memory_hierarchy/cache_latency/` |
| Sequential vs random access | Done | `labs/01_memory_hierarchy/sequential_vs_random/` |
| Cache line effect | Done | `labs/01_memory_hierarchy/cache_line_effect/` |
| TLB latency | Planned | `labs/02_virtual_memory/tlb_latency/` |
| Process tree | Planned | `labs/03_process_thread/process_tree/` |
| Context switch | Planned | `labs/03_process_thread/context_switch/` |

## Quick Start

Build the current completed lab:

```bash
make
```

Run it:

```bash
make run
```

Clean generated binaries:

```bash
make clean
```

## Repository Rule

Each completed lab should contain source code, build instructions, raw results, and analysis.

The standard lab questions are:

1. What is the question?
2. What is the OS/system concept?
3. How is the experiment designed?
4. How to reproduce it?
5. What is the result?
6. What does it prove or not prove?
