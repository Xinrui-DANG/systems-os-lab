# Systems OS Lab

A structured operating-system and computer-architecture lab repository focused on memory hierarchy, virtual memory, process isolation, scheduling, real-time interference, and heterogeneous runtime behavior.

一个围绕 **操作系统、计算机体系结构、运行时隔离、实时干扰、异构计算平台** 的系统实验仓库。

## Goal

This repository is not just a collection of OS demos. It is a systems portfolio path:

```text
Application behavior
  -> OS abstraction
  -> hardware mechanism
  -> timing effect
  -> isolation boundary
  -> heterogeneous runtime behavior
```

It starts from classic OS mechanisms and moves toward automotive heterogeneous trusted computing platforms:

```text
Memory
  -> Address Translation
  -> Process Isolation
  -> Syscall / Interrupt
  -> Scheduling
  -> Timing / Jitter
  -> Interference
  -> Isolation / Hypervisor
  -> Heterogeneous Runtime
```

## Structure

| Area | Purpose |
|---|---|
| `labs/` | Observable user-space experiments |
| `mini-kernel/` | Long-term Unix-like kernel project |
| `docs/` | Roadmap, concept maps, terminology, references |
| `results/` | Cross-module experiment outputs |
| `scripts/` | Repository-level automation |
| `references/` | Papers, books, standards, and reading notes |

## Roadmap

See [ROADMAP.md](ROADMAP.md) for the detailed learning plan.

## Current Progress

| Module | Status | Path |
|---|---:|---|
| Memory hierarchy | Done | `labs/01_memory_hierarchy/` |
| Virtual memory | Next | `labs/02_virtual_memory/` |
| Process/thread/scheduler | Planned | `labs/03_process_thread_scheduler/` |
| Syscall/interrupt | Planned | `labs/04_syscall_interrupt/` |
| Synchronization | Planned | `labs/05_synchronization/` |
| Storage/filesystem | Later | `labs/06_storage_filesystem/` |
| Heterogeneous computing | Planned | `labs/07_heterogeneous_computing/` |
| Real-time interference | Planned | `labs/08_realtime_interference/` |
| Isolation/virtualization | Planned | `labs/09_isolation_virtualization/` |
| Mini-kernel | Long-term | `mini-kernel/` |

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

Each module should also answer:

```text
How does this mechanism appear in Linux, RTOS, hypervisors, IOMMU, or heterogeneous platforms?
```
