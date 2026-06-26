# 01 Memory Hierarchy

This module studies how memory access patterns interact with caches, prefetching, cache lines, coherence, and DRAM latency.

This module connects observable program behavior to cache hierarchy, hardware prefetching, cache-line utilization, false sharing, and memory-level parallelism.

本模块把程序可观测行为与缓存层级、硬件预取、cache line 利用率、false sharing、内存级并行性连接起来。

Detailed Chinese result analysis: [results_analysis_zh.md](results_analysis_zh.md)

## Labs

| Lab | Status | Concept |
|---|---:|---|
| cache_latency | Done | pointer chasing, dependent load latency, working set size |
| sequential_vs_random | Done | hardware prefetching |
| cache_line_effect | Done | spatial locality and cache-line utilization |
| false_sharing | Done | cache coherence and multi-threaded writes |
| prefetch_effect | Done | software/hardware prefetch behavior |
| numa_or_l3_cluster | Done | local/remote cache or memory effects |

## Connection to Modern Systems

Memory hierarchy behavior is the foundation for later modules:

- TLB and page-table behavior in `02_virtual_memory`
- scheduler placement and migration in `03_process_thread_scheduler`
- lock contention and cache coherence in `05_synchronization`
- latency and jitter under interference in `08_realtime_interference`
- DMA, shared buffers, and bandwidth contention in `07_heterogeneous_computing`
- cache partitioning and freedom from interference in `09_isolation_virtualization`
