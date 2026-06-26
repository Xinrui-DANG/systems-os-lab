# Roadmap

This repository is a systems portfolio path, not a traditional OS course checklist.

The long-term direction is:

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

The high-end research target is accelerator partitioning with real-time reasoning:

```text
GPU/NPU resource sharing
  -> SM / partition-style allocation
  -> DMA / IOMMU isolation
  -> memory bandwidth and cache interference
  -> accelerator scheduling
  -> WCRT / WCET analysis
```

This is deliberately marked as long-term. The repository must first build the measurable OS foundations.

## Repository Layers

### Layer 1: Observable Labs

Small user-space experiments that expose OS and hardware behavior:

- memory hierarchy
- virtual memory
- process/thread/scheduler behavior
- syscall and interrupt-like behavior
- synchronization
- storage and page cache
- heterogeneous runtime concepts
- real-time interference
- isolation and virtualization

### Layer 2: Mini-Kernel

A long-term Unix-like kernel project for QEMU:

- boot
- timer interrupt
- preemptive scheduling
- virtual memory
- user/kernel separation
- basic syscalls
- simple driver
- ramfs or tiny file system
- minimal shell

### Layer 3: Systems Direction

Connect the basic mechanisms to the research/industry direction:

- runtime isolation
- hypervisor and IOMMU concepts
- automotive heterogeneous SoC platforms
- GPU/NPU/DMA interference
- accelerator resource partitioning
- SM / MIG / MPS-style sharing concepts
- WCRT/WCET degradation
- real-time jitter measurement

## Priority Order

1. `01_memory_hierarchy`
2. `02_virtual_memory`
3. `03_process_thread_scheduler`
4. `04_syscall_interrupt`
5. `08_realtime_interference`
6. `07_heterogeneous_computing`
7. `09_isolation_virtualization`
8. `mini-kernel`
9. `06_storage_filesystem`

File-system work is useful, but it is not the main line for this repository.

## Labs

| Module | Status | Main focus |
|---|---:|---|
| `00_toolchain` | Baseline implementation | gcc, make, perf, taskset, lscpu, sysfs/procfs |
| `01_memory_hierarchy` | Done | cache, prefetch, cache line, false sharing, L3 topology |
| `02_virtual_memory` | Scaffolded / Next | address layout, page fault, mmap, TLB, huge page, COW |
| `03_process_thread_scheduler` | Scaffolded | fork/exec, context switch, scheduling latency, priority inversion |
| `04_syscall_interrupt` | Scaffolded | syscall cost, signals, timers, interrupt-like latency |
| `05_synchronization` | Scaffolded | mutex, spinlock, atomic, producer-consumer |
| `06_storage_filesystem` | Scaffolded / Later | page cache, fsync, direct I/O, file descriptor |
| `07_heterogeneous_computing` | Scaffolded | DMA, shared buffer, command queue, IOMMU concept |
| `08_realtime_interference` | Scaffolded | periodic task latency, jitter, WCRT, interference |
| `09_isolation_virtualization` | Scaffolded | process isolation, mmap protection, seccomp, hypervisor concepts |

## Near-Term Execution

### Step 1: Keep `01_memory_hierarchy` as the template

This module already has source code, results, Chinese analysis, and follow-up topology analysis.

### Step 2: Build `02_virtual_memory`

Required experiments:

| Experiment | Question |
|---|---|
| `address_layout.c` | How are text/data/bss/heap/stack arranged? |
| `mmap_demo.c` | How does a file enter virtual address space? |
| `page_fault_demo.c` | How is a page fault triggered? |
| `tlb_pressure.c` | How does TLB pressure affect latency? |
| `hugepage_demo.c` | Why can huge pages reduce TLB pressure? |
| `page_replacement_sim.c` | How do FIFO/LRU/Clock differ? |
| `cow_fork_demo.c` | How does copy-on-write after fork happen? |
| `backing_store_sim.c` | Why does the OS load missing pages into DRAM instead of mapping flash directly? |

### Step 3: Build `03_process_thread_scheduler`

Required experiments:

| Experiment | Question |
|---|---|
| `fork_exec_demo.c` | How does process creation work? |
| `context_switch_cost.c` | What is context-switch overhead? |
| `pthread_create_cost.c` | How expensive is thread creation? |
| `scheduler_latency.c` | What latency does Linux scheduling introduce? |
| `priority_inversion.c` | How can priority inversion break real-time assumptions? |
| `mini_scheduler_sim.c` | How do FIFO/RR/Priority/EDF differ? |

### Step 4: Add Timing and Interference

`08_realtime_interference` should measure:

- min latency
- average latency
- p99 latency
- max latency
- jitter
- deadline misses
- WCRT

### Step 5: Add Heterogeneous Runtime Concepts

`07_heterogeneous_computing` should model:

- DMA
- shared buffers
- command queues
- accelerator runtime scheduling
- memory bandwidth interference
- fake GPU/NPU scheduler
- IOMMU-like isolation

## Final Target

This repository should demonstrate the ability to reason across:

```text
Application behavior
  -> OS abstraction
  -> hardware mechanism
  -> timing effect
  -> isolation boundary
  -> heterogeneous runtime behavior
```

That is the path from classic OS mechanisms toward an automotive heterogeneous trusted computing platform.

Long-term research question:

```text
Can CPU/GPU/NPU tasks share heterogeneous resources while bounding interference
and preserving real-time response-time requirements?
```

See [docs/accelerator_realtime_goal.md](docs/accelerator_realtime_goal.md).
