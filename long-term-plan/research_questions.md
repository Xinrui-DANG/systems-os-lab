# Research Questions

## Main Question

```text
Can CPU/GPU/NPU tasks share heterogeneous resources while bounding interference
and preserving real-time response-time requirements?
```

中文：

```text
异构系统能否在多个 CPU/GPU/NPU 任务并发运行时，
限制干扰并维持实时响应时间约束？
```

## Question Layers

| Layer | Question | Difficulty |
|---|---|---:|
| Tool usage | Can existing mechanisms such as MPS, MIG, stream priority, or context limits be used correctly? | Medium |
| Controlled experiments | How do SM limits, command queues, memory bandwidth, and copy engines affect latency? | Hard |
| Real-time analysis | Can response time be bounded under blocking, interference, and limited preemption? | Research-level |
| Safety argument | Can the system support freedom from interference for mixed-criticality workloads? | Research-level+ |

## Conceptual Model

A GPU-using real-time task is not a single CPU segment:

```text
Task_i =
  CPU segment
  + DMA/copy segment
  + GPU/NPU compute segment
  + synchronization
  + memory interference
```

A response-time model may need:

```text
R_i = CPU_i + Copy_i + GPU_i + Blocking_i + Interference_i
```

If runtime memory faults or migration are possible:

```text
R_i += page_fault_cost + migration_cost
```

That is why virtual memory, IOMMU, DMA, scheduling, and interference analysis all matter before this topic can be treated rigorously.
