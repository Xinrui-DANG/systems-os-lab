# Long-Term Goal: Accelerator Partitioning and Real-Time Analysis

This repository eventually builds toward understanding accelerator resource partitioning and real-time analysis in heterogeneous systems.

This is a long-term research direction, not a current implementation claim.

## Problem

GPU/NPU sharing is not just a virtual-memory problem. It crosses several layers:

```text
OS scheduling
  -> resource isolation
  -> GPU/NPU architecture
  -> DMA / IOMMU / memory management
  -> real-time interference
  -> WCRT / WCET analysis
```

The long-term question is:

```text
Can a heterogeneous system run multiple CPU/GPU/NPU tasks while bounding
interference and preserving real-time response-time requirements?
```

中文：

```text
异构系统能否在多个 CPU/GPU/NPU 任务并发运行时，
限制干扰并维持实时响应时间约束？
```

## GPU Sharing Concepts

| Concept | Meaning |
|---|---|
| temporal sharing / time slicing | workloads take turns using the GPU |
| spatial sharing / spatial multiplexing | workloads run concurrently on separated GPU resources |
| SM allocation / SM partitioning | assign Streaming Multiprocessors to workloads |
| GPU instance / MIG-style partition | split a GPU into more isolated instances |
| GPU context scheduling | schedule CUDA contexts or GPU process contexts |

## Why This Is Hard

A GPU-using real-time task is not a single CPU execution segment. A more realistic model is:

```text
Task_i =
  CPU segment
  + DMA/copy segment
  + GPU compute segment
  + synchronization
  + memory interference
```

A response-time model must reason about:

```text
R_i = CPU_i + Copy_i + GPU_i + Blocking_i + Interference_i
```

If page faults or migration happen at runtime:

```text
R_i += page_fault_cost + migration_cost
```

For hard real-time systems, uncontrolled page faults, managed-memory migration, and oversubscription are usually unacceptable.

## Relationship to This Repository

This direction is built gradually:

1. `00_toolchain`: measurement baseline
2. `01_memory_hierarchy`: cache, bandwidth, locality, false sharing
3. `02_virtual_memory`: page tables, TLB, page faults, backing store
4. `03_process_thread_scheduler`: scheduling, preemption, priority inversion
5. `08_realtime_interference`: jitter, WCRT, interference measurement
6. `07_heterogeneous_computing`: DMA, command queues, accelerator runtime model
7. `09_isolation_virtualization`: IOMMU, hypervisor, partitioning, isolation

## Reasonable Experiment Chain

The practical path is:

```text
1. CPU memory interference
2. page fault / TLB / cache jitter
3. accelerator command queue simulation
4. DMA / shared-buffer interference model
5. GPU kernel latency baseline, if hardware is available
6. concurrent GPU workload interference, if hardware is available
7. MPS / MIG / SM-limit experiments, if platform supports them
8. simple WCRT bound
9. explain why the bound is incomplete
```

## Non-Goals for the Current Stage

This repository does not currently claim:

- hard real-time GPU guarantees
- formal WCRT proof for GPU workloads
- physical SM binding implementation
- production hypervisor integration
- CUDA/MIG/MPS implementation
- safety certification

## Long-Term Target

The long-term target is not merely CUDA optimization. It is:

```text
heterogeneous resource scheduling
  + DMA/IOMMU isolation
  + memory-interference control
  + accelerator partitioning
  + real-time response-time analysis
```

This connects the repository to automotive heterogeneous trusted computing platforms, runtime isolation, hypervisors, GPU/NPU interference, and WCRT/WCET analysis.
