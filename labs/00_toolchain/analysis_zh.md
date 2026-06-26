# 00 Toolchain 中文分析

本模块的作用不是研究某一个单独的 OS 机制，而是为整个仓库建立测量基准。

后续实验中的 cache latency、TLB pressure、scheduler latency、jitter、WCRT 等结论都依赖以下前提：

```text
machine model
CPU topology
cache hierarchy
compiler version
compiler flags
CPU affinity
perf event availability
timing source
kernel version
```

中文对应为：

```text
机器型号
CPU 拓扑
缓存层级
编译器版本
编译参数
CPU 亲和性
perf 性能计数器可用性
计时源
内核版本
```

## 为什么需要这个模块？

系统实验不是纯算法题。相同代码在不同机器上可能得到完全不同的结果。

例如：

- L1/L2/L3 cache 大小不同，会改变 cache latency 阶梯；
- SMT / Hyper-Threading 会影响 false sharing 和调度抖动；
- NUMA 或 L3 cluster 会影响远端访问延迟；
- 编译参数会改变循环结构和内存访问模式；
- CPU migration 会污染延迟测量；
- perf 权限不足会导致无法采集硬件计数器。

因此，每个性能实验都应该能追溯到测量环境。

## 本模块当前检查什么？

| 项目 | 目的 |
|---|---|
| compiler version | 确认 gcc/clang 版本 |
| CFLAGS | 确认优化级别和 warning 设置 |
| CPU topology | 确认核心数、线程数、NUMA、cache |
| CPU affinity | 确认能否 pin 到固定 CPU |
| perf events | 确认 cycles、instructions、cache-misses 等是否可用 |
| kernel version | 记录系统调度和 perf 行为的环境背景 |

## 和后续模块的关系

| 后续模块 | 依赖本模块的原因 |
|---|---|
| `01_memory_hierarchy` | cache 层级、CPU pinning、perf counter |
| `02_virtual_memory` | TLB、page fault、huge page 行为与机器相关 |
| `03_process_thread_scheduler` | scheduler latency 依赖 kernel 和 CPU topology |
| `08_realtime_interference` | jitter/WCRT 测量必须控制 CPU placement |
| `07_heterogeneous_computing` | bandwidth interference 需要知道 memory/cache topology |

## 结论

`00_toolchain` 是整个仓库的测量可信度入口。

如果没有这个模块，后续实验结果只能算“跑出来了”，不能算“可解释”。
