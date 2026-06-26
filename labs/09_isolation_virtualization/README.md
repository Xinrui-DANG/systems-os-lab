# 09 Isolation / Virtualization

## Goal

Understand process isolation, memory protection, shared-memory channels, sandboxing, hypervisor concepts, and IOMMU/device isolation.

## Core Questions

1. What does a process boundary protect?
2. How does virtual memory support isolation?
3. How can shared memory intentionally cross isolation boundaries?
4. What does a hypervisor add beyond process isolation?
5. How does IOMMU protect memory from device DMA?

## Experiments

| Area | Experiment | Mechanism |
|---|---|---|
| `user_space_isolation` | `process_isolation_demo.c` | address-space boundary |
| `user_space_isolation` | `mmap_protection_demo.c` | page protection |
| `user_space_isolation` | `shared_memory_channel.c` | controlled sharing |
| `user_space_isolation` | `sandbox_sim.c` | restricted execution model |
| `hypervisor_concept` | `stage2_translation_note.md` | guest-to-host translation |
| `hypervisor_concept` | `jailhouse_case_study.md` | partitioning hypervisor |
| `hypervisor_concept` | `iommu_isolation_note.md` | DMA isolation |
| `hypervisor_concept` | `vm_interference_model.md` | interference model |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- process isolation
- address space
- page protection
- shared memory
- sandbox
- virtualization
- Type-1 hypervisor
- Stage-2 translation
- guest physical address
- host physical address
- IOMMU
- device passthrough
- freedom from interference

## Connection to Modern Systems

Goal: understand how hypervisors and IOMMUs isolate OS instances and devices in automotive heterogeneous SoCs.
