# 02 Virtual Memory

## Status

Scaffolded / Next implementation target. Experiments are designed but not implemented yet.

## Goal

Study how virtual addresses are translated, mapped, faulted in, and protected by the operating system and hardware MMU.

## Core Questions

1. How is a process virtual address space organized?
2. What happens when a page is not present?
3. How does `mmap()` connect files and anonymous memory to virtual memory?
4. How do TLB misses affect performance?
5. Why can huge pages reduce TLB pressure?
6. How does copy-on-write after `fork()` happen?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `address_layout.c` | Where are text/data/bss/heap/stack? | virtual address space |
| `mmap_demo.c` | How does file-backed mapping work? | `mmap`, page cache |
| `page_fault_demo.c` | How are minor faults triggered? | demand paging |
| `tlb_pressure.c` | How does page count affect latency? | TLB and page walk |
| `hugepage_demo.c` | Why do huge pages help? | larger translation granularity |
| `page_replacement_sim.c` | How do FIFO/LRU/Clock behave? | replacement algorithms |
| `cow_fork_demo.c` | What happens after `fork()`? | copy-on-write |
| `backing_store_sim.c` | How does OS-backed paging differ from direct flash mapping? | page fault and backing store |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- virtual address
- physical address
- page table
- PTE
- PFN
- TLB
- page fault
- backing store
- copy-on-write
- huge page

## Connection to Modern Systems

Virtual memory is the basis of process isolation, hypervisor stage-2 translation, IOMMU mappings, memory protection, demand paging, and shared-memory communication with devices.
