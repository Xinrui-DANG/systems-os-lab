# Experiment Chain

This is the practical bridge from the current repository to accelerator partitioning and real-time analysis.

## Stage 0: Measurement Baseline

Folder:

```text
labs/00_toolchain
```

Goal:

- machine profile
- compiler version
- CPU/cache topology
- perf counter availability
- CPU affinity behavior

## Stage 1: CPU Memory and Cache Interference

Folder:

```text
labs/01_memory_hierarchy
```

Goal:

- cache latency
- hardware prefetching
- cache line utilization
- false sharing
- L3 cluster behavior

## Stage 2: Virtual Memory Determinism

Folder:

```text
labs/02_virtual_memory
```

Goal:

- TLB pressure
- page faults
- copy-on-write
- backing store model
- huge pages
- avoiding runtime faults in critical paths

## Stage 3: Scheduler and Timing

Folders:

```text
labs/03_process_thread_scheduler
labs/08_realtime_interference
```

Goal:

- context-switch cost
- scheduling latency
- periodic task jitter
- WCRT-style reporting
- CPU/memory/cache interference

## Stage 4: Accelerator Runtime Model

Folder:

```text
labs/07_heterogeneous_computing
```

Goal:

- DMA model
- shared buffer model
- command queue simulation
- accelerator runtime simulation
- memory bandwidth interference
- fake GPU scheduler

## Stage 5: Isolation and Partitioning

Folder:

```text
labs/09_isolation_virtualization
```

Goal:

- process isolation
- page protection
- shared-memory channels
- IOMMU concept
- hypervisor partitioning notes
- VM interference model

## Stage 6: Hardware-Dependent Accelerator Experiments

Only when suitable hardware and drivers are available:

- GPU kernel latency baseline
- concurrent GPU workload interference
- MPS or MIG behavior
- SM-limit or partition-style behavior if supported
- copy-engine interference
- unified memory migration impact

## Stage 7: Simple WCRT Model

Initial model:

```text
R_i = CPU_i + Copy_i + GPU_i + Blocking_i + Interference_i
```

Then document why this model is incomplete:

- GPU internal scheduling is partially opaque
- kernel preemption can be limited
- memory interference is shared
- page migration can break determinism
- DVFS and thermal throttling affect latency
