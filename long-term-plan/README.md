# Long-Term Plan

This folder collects long-term research ideas that guide the repository but are not current implementation claims.

The main repository should stay honest about what is implemented today:

```text
Implemented now:
  00_toolchain
  01_memory_hierarchy

Next foundation:
  02_virtual_memory
  03_process_thread_scheduler
  08_realtime_interference

Long-term direction:
  accelerator partitioning
  GPU/NPU interference
  DMA/IOMMU isolation
  hypervisor-backed isolation
  WCRT/WCET-aware heterogeneous scheduling
```

## Plans

| File | Purpose |
|---|---|
| [accelerator_partitioning_realtime.md](accelerator_partitioning_realtime.md) | Main long-term GPU/NPU partitioning and real-time analysis goal |
| [research_questions.md](research_questions.md) | Research questions and difficulty levels |
| [experiment_chain.md](experiment_chain.md) | Practical experiment path from current labs to accelerator interference |
| [non_goals.md](non_goals.md) | What this repository does not currently claim |

## Why This Folder Exists

GPU/NPU space sharing, SM allocation, MIG/MPS-style partitioning, and real-time response-time analysis are high-level research topics.

They should guide the repository direction, but they should not be mixed with implemented module documentation in a way that suggests the current repository already provides those guarantees.

This folder keeps that separation clear.
