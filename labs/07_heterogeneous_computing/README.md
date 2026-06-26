# 07 Heterogeneous Computing

## Goal

Study how an operating system or runtime manages accelerators through shared memory, command queues, DMA, IOMMU-like isolation, scheduling, and interference control.

## Core Questions

1. How can a device access memory without CPU load/store instructions?
2. How do CPU and accelerator share buffers?
3. How does a command queue represent work submission?
4. How can accelerator work interfere with CPU timing?
5. What does IOMMU-style isolation protect?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `dma_model.c` | What does DMA conceptually do? | direct memory access |
| `shared_buffer_demo.c` | How do CPU and accelerator share data? | shared buffer |
| `command_queue_sim.c` | How is work submitted to a device? | command queue |
| `accelerator_runtime_sim.c` | How does a runtime submit and wait? | runtime scheduling |
| `memory_bandwidth_interference.c` | How can accelerators disturb CPU latency? | bandwidth contention |
| `fake_gpu_scheduler.c` | How do multiple jobs compete for one accelerator? | device scheduling |
| `iommu_concept_sim.c` | How can DMA be restricted? | IOMMU-like isolation |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- heterogeneous computing
- accelerator
- DMA
- shared buffer
- command queue
- runtime
- IOMMU
- memory bandwidth contention
- interference isolation

## Connection to Modern Systems

This module connects OS mechanisms to GPU/NPU runtimes, DMA isolation, automotive SoCs, and interference between untrusted AI workloads and real-time safety domains.
