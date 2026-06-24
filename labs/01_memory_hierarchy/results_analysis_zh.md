# 01 Memory Hierarchy 中文结果分析

本模块的目标不是单纯写几个 benchmark，而是用一组逐步递进的实验，把 CPU cache、cache line、hardware prefetch、false sharing、cache coherence、L3 cluster 等概念和可观测的程序行为连接起来。

从操作系统和系统软件角度看，程序性能不只由算法复杂度决定，还受到以下因素共同影响：

```text
address pattern
data layout
cache hierarchy
cache line utilization
cache coherence granularity
CPU topology
scheduler placement
memory placement
TLB behavior
```

中文对应为：

```text
地址访问模式
数据布局
缓存层级
缓存行利用率
缓存一致性粒度
CPU 拓扑
调度位置
内存放置
TLB 行为
```

本模块当前包含以下实验：

| 实验                     | 核心问题                                           | 当前结论                                                         |
| ---------------------- | ---------------------------------------------- | ------------------------------------------------------------ |
| `cache_latency`        | working set 变大后，访问延迟如何跨过 L1/L2/L3/DRAM 边界      | random pointer chasing 可以测出明显的缓存层级延迟阶梯                       |
| `sequential_vs_random` | 为什么顺序访问远快于随机访问                                 | 顺序访问可被硬件预取、乱序执行和内存级并行性优化；随机依赖访问暴露真实 load latency             |
| `cache_line_effect`    | stride 如何影响 cache line 利用率                     | stride 越大，每条 64B cache line 中真正有用的数据越少；大 stride 还可能引入 TLB 压力 |
| `false_sharing`        | 两个线程写不同变量为什么仍会互相拖慢                             | cache coherence 以 cache line 为单位工作，不以变量为单位工作                 |
| `prefetch_effect`      | 软件预取和硬件预取分别什么时候有效                              | 对简单顺序流，硬件预取已经很强，手动 prefetch 不一定更快                            |
| `numa_or_l3_cluster`   | 换 CPU 后是否出现 local/remote cache 或 L3 cluster 差异 | 当前机器没有稳定证明跨 L3 cluster 更慢，结论是负结果或不确定结果                       |

---

## 0. 实验环境

当前主要测试机器：

| 项目               | 信息                                      |
| ---------------- | --------------------------------------- |
| CPU              | AMD Ryzen 9 8945HX with Radeon Graphics |
| Architecture     | x86_64                                  |
| Logical CPUs     | 32                                      |
| Physical cores   | 16                                      |
| Threads per core | 2                                       |
| Address sizes    | 48 bits physical, 48 bits virtual       |
| NUMA nodes       | 1                                       |
| L1d              | 32 KiB per core                         |
| L1i              | 32 KiB per core                         |
| L2               | 1 MiB per core                          |
| L3               | 32 MiB × 2 instances = 64 MiB total     |
| Cache line size  | 64 B                                    |
| Compiler flags   | `-O2 -march=native`                     |
| CPU pinning      | `taskset`                               |
| Profiling tool   | `perf stat`                             |

通过 sysfs 查看 `cpu0` 的 cache topology：

```text
L1d: 32 KiB, 8-way, 64 sets, 64 B line
L1i: 32 KiB, 8-way, 64 sets, 64 B line
L2 : 1024 KiB, 8-way, 2048 sets, 64 B line
L3 : 32768 KiB, 16-way, 32768 sets, 64 B line
```

缓存容量满足：

```text
cache size = line size × ways × sets
```

例如 L1 data cache：

```text
64 B × 8 ways × 64 sets = 32768 B = 32 KiB
```

再例如 L2 cache：

```text
64 B × 8 ways × 2048 sets = 1048576 B = 1024 KiB = 1 MiB
```

再例如当前核心可见的 L3 cache instance：

```text
64 B × 16 ways × 32768 sets = 33554432 B = 32768 KiB = 32 MiB
```

需要注意：整颗 CPU 的 L3 总量是 64 MiB，但它分成 2 个 L3 instances。一个 pinned thread 主要观察到的是当前 CPU 所属 cluster 的本地 32 MiB L3，而不是整颗 CPU 的 64 MiB L3。

---

## 0.1 复现命令

查看 CPU 和 cache 信息：

```bash
lscpu
lscpu --caches
```

查看 `cpu0` 的详细 cache sysfs 信息：

```bash
for c in /sys/devices/system/cpu/cpu0/cache/index*; do
    echo "==== $c ===="
    echo -n "level: "; cat "$c/level"
    echo -n "type: "; cat "$c/type"
    echo -n "size: "; cat "$c/size"
    echo -n "line size: "; cat "$c/coherency_line_size"
    echo -n "ways: "; cat "$c/ways_of_associativity"
    echo -n "sets: "; cat "$c/number_of_sets"
    if [ -f "$c/shared_cpu_list" ]; then
        echo -n "shared_cpu_list: "; cat "$c/shared_cpu_list"
    fi
    echo
done
```

编译并运行实验：

```bash
make
taskset -c 2 ./cache_latency
```

使用 `perf stat` 观察硬件性能计数器：

```bash
perf stat \
  -e cycles,instructions,L1-dcache-loads,L1-dcache-load-misses \
  taskset -c 2 ./cache_latency
```

在当前 AMD 平台上，通用事件名：

```text
LLC-loads
LLC-load-misses
```

显示为：

```text
<not supported>
```

这不是程序错误，而是当前内核和平台的 perf event mapping 没有直接支持这些 generic LLC events。可以用以下命令查看平台实际支持的 cache/L3 相关事件：

```bash
perf list | grep -i -E "l3|llc|cache|miss"
```

---

# 1. cache_latency：缓存层级延迟

## 1.1 实验问题

`cache_latency` 想回答的问题是：

```text
当 working set 从 L1 能装下，逐步变成只有 L2、L3、DRAM 才能容纳时，
一次随机依赖 load 的成本如何变化？
```

这个实验的核心意义是建立 memory hierarchy 的第一条主线：

```text
缓存层级不是抽象结构，而是能从用户态程序里测出来的延迟阶梯。
```

---

## 1.2 实验方法

`cache_latency` 使用 random pointer chasing：

```c
idx = next[idx];
```

这个访问模式的关键是：下一次访问的地址必须等当前 load 完成之后才知道。因此 CPU 很难提前预取，也很难并行发出多个独立内存请求。

实验步骤：

1. 构造一个随机排列。
2. 把数组下标连接成一个随机环。
3. 从 4 KiB 到 512 MiB 逐步增大 working set。
4. 每个 working set 运行固定次数的 dependent load。
5. 输出 `ns/access`。

这里的 `ns/access` 更接近：

```text
dependent-load latency
依赖加载延迟
```

因为访问之间存在强数据依赖。

---

## 1.3 实验结果

在 AMD Ryzen 9 8945HX 上，固定到 logical CPU 2：

```bash
taskset -c 2 ./cache_latency
```

结果：

```text
 Working set       ns/access
        4 KiB           0.954
        8 KiB           0.955
       16 KiB           0.983
       32 KiB           1.004
       64 KiB           2.043
      128 KiB           2.493
      256 KiB           2.689
      512 KiB           3.696
     1024 KiB           5.024
     2048 KiB           7.894
     4096 KiB           9.637
     8192 KiB          10.509
    16384 KiB          13.557
    32768 KiB          49.049
    65536 KiB          84.851
   131072 KiB          95.222
   262144 KiB         104.139
   524288 KiB         105.432
```

---

## 1.4 结果解释

当前机器的 cache hierarchy 是：

```text
L1d = 32 KiB
L2  = 1 MiB
L3  = 32 MiB local instance
```

实验结果可以分成几个区域：

|    Working set |     ns/access | 主要层级            | 解释                    |
| -------------: | ------------: | --------------- | --------------------- |
| 4 KiB – 32 KiB | ~0.95–1.00 ns | L1d             | 工作集可以放入 L1 data cache |
| 64 KiB – 1 MiB |   ~2.0–5.0 ns | L2              | 超过 L1，主要进入 L2         |
| 2 MiB – 16 MiB |  ~7.9–13.6 ns | L3              | 超过 L2，主要进入 L3         |
|         32 MiB |        ~49 ns | L3 边界 / DRAM 混合 | 接近本地 L3 instance 容量   |
|      64 MiB 以上 |    ~85–105 ns | DRAM            | 大量访问主内存               |

第一个明显跳变：

```text
32 KiB  → 64 KiB
1.004 ns → 2.043 ns
```

对应 L1d 容量边界。

第二个明显变化：

```text
1024 KiB → 2048 KiB
5.024 ns → 7.894 ns
```

对应 L2 容量边界。

第三个大跳变：

```text
16384 KiB → 32768 KiB
13.557 ns → 49.049 ns
```

对应本地 L3 cache instance 的容量边界。

超过 64 MiB 后：

```text
65536 KiB → 524288 KiB
84.851 ns → 105.432 ns
```

主要进入 DRAM latency 区间。

---

## 1.5 perf stat 分析

运行：

```bash
perf stat \
  -e cycles,instructions,L1-dcache-loads,L1-dcache-load-misses \
  taskset -c 2 ./cache_latency
```

得到：

```text
119,366,617,668      cycles:u
 11,542,440,853      instructions:u
  3,884,201,524      L1-dcache-loads:u
    594,898,046      L1-dcache-load-misses:u

23.695236873 seconds time elapsed
22.951234000 seconds user
 0.649310000 seconds sys
```

派生指标：

```text
IPC = instructions / cycles
    = 11.54B / 119.37B
    ≈ 0.097 instructions/cycle
```

```text
CPI = cycles / instructions
    = 119.37B / 11.54B
    ≈ 10.34 cycles/instruction
```

```text
L1D miss rate
= L1-dcache-load-misses / L1-dcache-loads
= 594.90M / 3.884B
≈ 15.32%
```

```text
Average CPU frequency
= cycles / elapsed time
= 119.37B / 23.695s
≈ 5.04 GHz
```

解释：

1. IPC 只有约 0.097，说明程序严重受 memory latency 限制。
2. L1D miss rate 约 15.3%，符合 random pointer chasing 的预期。
3. 平均频率约 5.04 GHz，因此 80–105 ns 的 DRAM 随机依赖访问大约对应 400–525 cycles。
4. 当前 perf 统计的是整个程序，不只包括 `t0` 到 `t1` 之间的测量 loop，也包括初始化、shuffle、构造随机环、malloc/free、warm-up、printf 等代码。因此这些 perf 数字适合说明整体 workload 的 memory-bound 特征，但不能精确对应每一个 working set 的单独性能。

---

## 1.6 cache_latency 的核心结论

`cache_latency` 清楚暴露了 Ryzen 9 8945HX 的本地缓存层级：

```text
L1d boundary: around 32 KiB
L2 boundary : around 1 MiB
L3 boundary : around 32 MiB
DRAM region : above local L3 capacity
```

该实验说明：

```text
random dependent load 可以让 CPU 难以隐藏内存访问延迟，
因此能够从用户态程序中测出 L1/L2/L3/DRAM 的延迟阶梯。
```

---

# 2. sequential_vs_random：硬件预取和内存级并行性

## 2.1 实验问题

`sequential_vs_random` 想回答：

```text
为什么同样是读内存，顺序访问远快于随机访问？
```

这个实验对比两种访问模式：

顺序访问：

```c
sum += data[idx];
idx = (idx + 1) & mask;
```

随机依赖访问：

```c
idx = next[idx];
```

---

## 2.2 实验现象

sample run 中：

```text
sequential access: approximately 0.38–0.45 ns/access
random chasing   : up to around 100 ns/access for large working sets
```

这个差距说明：

1. 顺序访问有稳定的地址流。
2. CPU hardware prefetcher 可以提前把未来 cache line 拉进来。
3. 顺序访问可以利用 memory-level parallelism。
4. 乱序执行可以让多个独立内存请求同时进行。
5. random pointer chasing 的下一个地址不可提前知道，因此预取器和乱序执行很难发挥作用。

---

## 2.3 latency 与 throughput 的区别

这里需要特别区分：

```text
latency    = 单次操作从发出到完成的时间
throughput = 单位时间内完成多少操作
```

`cache_latency` 中的 random pointer chasing 更接近测 dependent-load latency。

而 `sequential_vs_random` 中的 sequential access 的 `ns/access` 更接近 throughput 指标，而不是单次真实 load latency。

原因是顺序访问可以被 CPU 隐藏大量延迟：

```text
hardware prefetching
out-of-order execution
memory-level parallelism
cache line reuse
loop optimization
```

因此，顺序访问的 `0.38–0.45 ns/access` 不表示一次真实 DRAM load 只需要 0.4 ns，而是表示 CPU 通过预取和并行化让平均吞吐达到这个水平。

---

## 2.4 核心结论

这个实验说明：

```text
同样是读内存，访问模式决定 CPU 能否隐藏内存延迟。
```

顺序访问：

```text
可预测
可预取
可并行
吞吐高
```

随机依赖访问：

```text
不可提前知道下一个地址
难以预取
难以并行
暴露真实 load latency
```

因此，该实验不是在测 DRAM 最高带宽，也不是在测单次真实 DRAM latency。它展示的是：

```text
访问模式可以让 CPU 隐藏大量访存延迟，也可以迫使 CPU 暴露延迟。
```

---

# 3. cache_line_effect：空间局部性和 cache line 利用率

## 3.1 实验问题

`cache_line_effect` 想回答：

```text
为什么 stride 会显著影响内存访问性能？
```

实验使用不同 stride 读取同一个 256 MiB byte array：

```text
1B, 2B, 4B, 8B, 16B, 32B, 64B, 128B, 256B, 512B, 1024B, 4096B
```

---

## 3.2 实验现象

sample run 中：

```text
stride 1B:    0.233 ns/access, line utilization 100.00%
stride 64B:   1.955 ns/access, line utilization   1.56%
stride 4096B: 6.278 ns/access, line utilization   1.56%
```

当前机器的 cache line size 是：

```text
64 B
```

CPU 从内存拉数据进 cache 时，通常不是只拉一个 byte，而是拉一整条 cache line。

---

## 3.3 结果解释

如果程序按：

```text
stride = 1B
```

访问，那么一条 64B cache line 里的 64 个 byte 最终都可能被用到：

```text
line utilization = 64 / 64 = 100%
```

如果程序按：

```text
stride = 64B
```

访问，那么每次只用一条 cache line 里的 1 个 byte，其余 63 个 byte 被搬进 cache 但没有被使用：

```text
line utilization = 1 / 64 = 1.56%
```

所以这个实验说明：

```text
cache miss 的成本不只取决于是否 miss，
还取决于 miss 之后搬进来的 cache line 有多少数据被真正利用。
```

这就是 spatial locality 的实际含义。

---

## 3.4 stride = 4096B 的特殊性

`stride = 4096B` 不只是 cache line utilization 问题，还可能引入 TLB 压力。

原因是 Linux 上常见 page size 是：

```text
4 KiB = 4096 B
```

当 stride 等于 4096B 时，每次访问通常落到一个新页面上。这会增加：

```text
DTLB pressure
TLB miss risk
page-walk overhead
```

因此，`stride = 4096B` 的性能下降可能同时来自：

1. cache line 利用率低；
2. 访问跨页面；
3. TLB miss 增加；
4. page table walk 开销上升。

这也是后续 `02_virtual_memory/tlb_latency` 模块需要单独研究的问题。

---

## 3.5 核心结论

`cache_line_effect` 说明：

```text
空间局部性不是抽象概念，而是 cache line 级别的数据利用率问题。
```

程序性能不只取决于访问了多少数据，还取决于：

```text
每次搬进 cache 的 64B 中，有多少字节被真正使用。
```

---

# 4. false_sharing：cache coherence 的真实粒度

## 4.1 实验问题

`false_sharing` 想回答：

```text
两个线程明明写的是不同变量，为什么仍然会互相拖慢？
```

核心原因是：

```text
cache coherence 以 cache line 为单位工作，不以变量为单位工作。
```

---

## 4.2 实验方法

第一种数据布局：两个 counter 靠在一起。

```c
typedef struct {
    volatile uint64_t a;
    volatile uint64_t b;
} shared_counters_t;
```

第二种数据布局：把每个 counter padding 到 64B。

```c
typedef struct __attribute__((aligned(64))) {
    volatile uint64_t value;
    char padding[64 - sizeof(uint64_t)];
} padded_counter_t;
```

两个线程分别写自己的 counter。逻辑上它们没有共享同一个变量，但如果两个 counter 位于同一条 cache line，cache coherence 协议仍然会把整条 cache line 的 ownership 在 CPU 之间来回转移。

---

## 4.3 volatile 的作用和限制

这里使用 `volatile` 的目的主要是防止编译器消除写操作，使 benchmark 中的写入真实发生。

但是需要注意：

```text
volatile is not synchronization.
volatile 不是线程同步机制。
```

`volatile` 不等价于：

```text
atomic
mutex
memory barrier
happens-before relation
```

真实并发程序中，如果 counter 需要跨线程语义，应使用 C11 atomic 或锁。

但对于 false sharing benchmark，重点不是构建正确的共享计数器，而是制造高频写入并观察 cache line ownership 迁移成本。

---

## 4.4 实验现象

sample run 中，padding 后约：

```text
6.75x speedup
```

这说明 false sharing 对多线程高频写入的影响很大。

没有 padding 时：

```text
Thread A writes counter a
Thread B writes counter b
a and b are in the same cache line
```

因此 CPU 之间会反复争夺同一条 cache line 的 ownership。

padding 后：

```text
counter a is in one cache line
counter b is in another cache line
```

两个线程写入时不再频繁抢同一条 cache line，性能显著提升。

---

## 4.5 核心结论

`false_sharing` 说明：

```text
多线程程序里的“共享”不只发生在变量层面，
也发生在 cache line 层面。
```

性能优化时，数据布局和 cache line 对齐很重要。尤其是以下结构很容易出现 false sharing：

```text
per-thread counter
queue metadata
lock state
ring buffer index
statistics counter
hot-path state variable
```

对应中文：

```text
每线程计数器
队列元数据
锁状态
环形缓冲区索引
统计计数器
热路径状态变量
```

---

# 5. prefetch_effect：软件预取不一定更快

## 5.1 实验问题

`prefetch_effect` 想回答：

```text
软件预取是否一定比普通顺序访问更快？
```

实验对比三种模式：

普通顺序访问：

```c
sum += data[i];
```

软件预取：

```c
__builtin_prefetch(&data[i + distance]);
sum += data[i];
```

随机 pointer chasing：

```c
idx = next[idx];
```

---

## 5.2 实验现象

sample run：

```text
sequential          0.237 ns/access
software_prefetch   0.344 ns/access
random_chase      101.448 ns/access
```

结果看起来有点反直觉：手动 software prefetch 比普通 sequential 更慢。

---

## 5.3 结果解释

原因是：

1. 顺序访问本身已经非常容易被硬件预取器识别。
2. `__builtin_prefetch` 额外增加了指令和前端压力。
3. 如果 prefetch distance 不合适，可能太早、太晚，或者污染 cache。
4. 对已经很规则的 streaming access，手写 prefetch 不一定比硬件更聪明。
5. random pointer chasing 依然很慢，因为下一个地址必须依赖当前 load 的结果。

所以这个实验的结论不是：

```text
software prefetch 没用
```

而是：

```text
software prefetch 只有在硬件预取器无法充分处理，
且程序能足够早知道未来地址时，才可能有价值。
```

---

## 5.4 software prefetch 可能有效的条件

software prefetch 通常需要同时满足：

1. 未来地址可以提前计算出来；
2. prefetch distance 足够覆盖内存延迟；
3. prefetch 不会污染 cache；
4. 额外 prefetch 指令的开销低于节省的 memory stall；
5. 硬件 prefetcher 无法自动识别该访问模式。

英文表述：

```text
Software prefetch is useful only when future addresses are known early enough,
the prefetch distance is well tuned,
and the extra instructions do not outweigh the saved memory stalls.
```

---

## 5.5 核心结论

`prefetch_effect` 说明：

```text
硬件预取对规则顺序流已经很强，
手动 software prefetch 不是万能加速器。
```

它更适合那些：

```text
不完全规则
但未来地址仍可提前计算
硬件预取器无法充分处理
```

的访问模式。

---

# 6. numa_or_l3_cluster：topology probe 与负结果

## 6.1 实验问题

`numa_or_l3_cluster` 最初想回答：

```text
如果数据在 CPU A 上 warm up，然后切到 CPU B 上访问，
是否会出现 local/remote cache 或 L3 cluster 差异？
```

当前机器：

```text
AMD Ryzen 9 8945HX
32 logical CPUs
16 physical cores
2 threads per core
1 NUMA node
2 L3 instances
```

由于只有 1 个 NUMA node，这个实验更准确地说不是完整 NUMA benchmark，而是：

```text
L3 cluster locality probe
L3 cluster 拓扑探测实验
```

---

## 6.2 实验方法

程序做法：

1. 构造随机 pointer-chasing cycle。
2. pin 到 warm CPU，先走一遍 pointer chain。
3. pin 到 measure CPU，再计时访问。
4. 对比 same CPU、SMT sibling、same L3、other L3。

关键 CPU placement：

| 关系                           | warm CPU | measure CPU |
| ---------------------------- | -------: | ----------: |
| same logical CPU             |        0 |           0 |
| SMT sibling                  |        0 |          16 |
| different core, same L3      |        0 |           1 |
| different core, different L3 |        0 |           8 |

---

## 6.3 实验结果

重复 7 次的 16 MiB 结果：

```text
same CPU:  13.113 ns/access median
same L3:   12.619 ns/access median
other L3:  12.979 ns/access median
```

当前结果没有稳定证明：

```text
跨 L3 cluster 一定更慢。
```

working-set sweep 也没有给出清晰单调趋势。

---

## 6.4 为什么这是合理的负结果

这个结果不是失败，而是一个重要结论：

```text
当前 benchmark 在这台机器上没有隔离出强 L3-cluster penalty。
```

可能原因包括：

1. 机器只有 1 个 NUMA node，不是 remote NUMA DRAM 测试。
2. 16 MiB working set 可能仍然适合当前 L3 结构。
3. 64 MiB 时可能已经受 DRAM、频率和调度噪声主导。
4. CPU frequency scaling 会影响短时间 benchmark。
5. 单纯 timing 无法区分 L1/L2/L3/DRAM/频率变化。
6. 跨 L3 的差异可能小于 run-to-run noise。
7. cache coherence 和 data migration 的实际行为比简单 local/remote 模型复杂。
8. warm-up 后数据是否稳定保留在预期 cache 层级中，并不容易仅靠 timing 证明。

---

## 6.5 后续改进方向

后续需要：

1. 固定 CPU 频率，或至少记录频率；
2. 增加 repeated trials；
3. 使用 median、p95、standard deviation；
4. 使用 perf counters；
5. 在多 NUMA node 机器上用 `numactl` 控制 memory placement；
6. 分别测试 SMT sibling、same L3、different L3、different NUMA node；
7. 将 warm-up 和 measure 阶段分别计数；
8. 尝试更长运行时间，降低短时间噪声；
9. 结合 `/sys/devices/system/cpu/cpu*/cache/index3/shared_cpu_list` 明确 L3 cluster membership。

查看 L3 shared CPU list：

```bash
for f in /sys/devices/system/cpu/cpu*/cache/index3/shared_cpu_list; do
    echo "$f: $(cat "$f")"
done | sort -u
```

---

## 6.6 核心结论

`numa_or_l3_cluster` 当前应被定位为：

```text
topology-sensitive probe
拓扑敏感型探测实验
```

而不是完整证明。

它的当前结论是：

```text
在当前 AMD Ryzen 9 8945HX、1 NUMA node、2 L3 instances 的机器上，
该 benchmark 没有稳定隔离出跨 L3 cluster 的额外惩罚。
```

负结果仍然有价值，因为它说明：

```text
topology 实验必须结合 CPU 拓扑、重复运行、频率控制和硬件计数器，
不能只凭一次 timing 下结论。
```

---

# 7. 模块总体结论

这一组实验可以串成一条完整逻辑链：

## 7.1 cache_latency

```text
random dependent load 会暴露缓存层级延迟。
```

它展示了：

```text
L1d → L2 → L3 → DRAM
```

之间的延迟阶梯。

---

## 7.2 sequential_vs_random

```text
访问模式决定 CPU 能否隐藏延迟。
```

顺序访问可以被 hardware prefetch、out-of-order execution 和 memory-level parallelism 优化；random pointer chasing 则会暴露 dependent-load latency。

---

## 7.3 cache_line_effect

```text
cache line 的利用率决定 spatial locality 的收益。
```

CPU 搬运数据的基本粒度通常是 64B cache line，而不是单个 byte 或单个变量。

---

## 7.4 false_sharing

```text
cache coherence 的真实粒度是 cache line。
```

两个线程即使写不同变量，只要这些变量落在同一条 cache line 上，就可能发生 false sharing。

---

## 7.5 prefetch_effect

```text
硬件预取对规则访问很强，手动 software prefetch 不是万能加速器。
```

software prefetch 只有在未来地址可提前知道、prefetch distance 合适、且硬件预取器无法充分处理时才可能有效。

---

## 7.6 numa_or_l3_cluster

```text
CPU topology 实验需要重复运行、控制变量和硬件计数器。
```

单次 timing 结果不足以证明 L3 cluster 或 NUMA penalty。

---

# 8. 从 OS 角度的意义

从 OS 和系统软件角度看，这些实验共同说明：

```text
程序性能不是只由算法复杂度决定，
还由地址模式、数据布局、缓存层级、CPU 拓扑、调度位置和内存放置共同决定。
```

这直接连接到后续模块：

| 后续模块                          | 与 memory hierarchy 的关系                                 |
| ----------------------------- | ------------------------------------------------------ |
| `02_virtual_memory`           | TLB、page table、page fault 会影响地址翻译和访存延迟                 |
| `03_process_thread`           | 进程和线程决定执行上下文                                           |
| `04_scheduler_affinity`       | 调度器决定线程在哪个 CPU 上运行，从而影响 cache locality                 |
| `05_synchronization`          | lock、atomic、false sharing 会影响 cache coherence traffic  |
| `06_ipc`                      | IPC 可能引入 copy、cache pollution 和 context switch         |
| `07_filesystem_io`            | page cache 把文件 I/O 和内存系统连接起来                           |
| `08_signal_interrupt_timer`   | timer jitter 和 wakeup latency 受调度和 cache 状态影响          |
| `11_rtos_experiments`         | 实时任务的 jitter/WCRT 与 cache、TLB、调度干扰有关                   |
| `12_virtualization_isolation` | hypervisor、cgroup、IOMMU、cache partitioning 都与隔离和资源竞争有关 |

因此，`01_memory_hierarchy` 是后续 OS 实验的基础模块。

---

# 9. 关键词对照

| English                  | Deutsch                      | 中文      |
| ------------------------ | ---------------------------- | ------- |
| memory hierarchy         | Speicherhierarchie           | 存储层级    |
| cache hierarchy          | Cache-Hierarchie             | 缓存层级    |
| cache line               | Cache-Zeile                  | 缓存行     |
| cache hit                | Cache-Treffer                | 缓存命中    |
| cache miss               | Cache-Fehltreffer            | 缓存未命中   |
| L1 data cache            | L1-Datencache                | 一级数据缓存  |
| L1 instruction cache     | L1-Instruktionscache         | 一级指令缓存  |
| unified cache            | vereinheitlichter Cache      | 统一缓存    |
| last-level cache         | Last-Level-Cache             | 最后一级缓存  |
| working set              | Arbeitsmenge                 | 工作集     |
| pointer chasing          | Zeigerverfolgung             | 指针追逐    |
| dependent load           | abhängiger Load              | 依赖加载    |
| latency                  | Latenz                       | 延迟      |
| throughput               | Durchsatz                    | 吞吐      |
| hardware prefetcher      | Hardware-Prefetcher          | 硬件预取器   |
| software prefetch        | Software-Prefetch            | 软件预取    |
| spatial locality         | räumliche Lokalität          | 空间局部性   |
| temporal locality        | zeitliche Lokalität          | 时间局部性   |
| false sharing            | falsches Teilen              | 伪共享     |
| cache coherence          | Cache-Kohärenz               | 缓存一致性   |
| CPU affinity             | CPU-Affinität                | CPU 亲和性 |
| scheduler                | Scheduler                    | 调度器     |
| NUMA                     | Non-Uniform Memory Access    | 非一致内存访问 |
| TLB                      | Translation Lookaside Buffer | 地址转换缓存  |
| page walk                | Seitentabellenlauf           | 页表遍历    |
| page fault               | Seitenfehler                 | 缺页异常    |
| memory-level parallelism | Speicherparallelität         | 内存级并行性  |

---

# 10. 当前模块的限制

当前实验仍然有以下限制：

1. 多数实验依赖 timing，缺少每个 working set 的独立 perf counters。
2. CPU frequency scaling 可能影响结果。
3. Turbo boost 可能导致不同 run 之间频率不同。
4. 后台任务和系统中断可能引入噪声。
5. 当前机器只有 1 个 NUMA node，不能证明真正的 remote NUMA memory latency。
6. AMD 平台上的通用 `LLC-loads` / `LLC-load-misses` perf events 不可用，需要进一步查平台特定事件。
7. sequential benchmark 的 `ns/access` 更接近吞吐指标，不应直接解释为单次 load latency。
8. `stride = 4096B` 的实验可能同时混入 cache line effect 和 TLB effect。
9. false sharing benchmark 使用 `volatile` 只是为了防止编译器优化，不代表并发同步语义。
10. L3 cluster 实验目前只能作为 topology probe，不能作为强结论。

---

# 11. 后续改进计划

后续可以补充：

1. 为每个 working set 单独运行 benchmark 和 perf stat；
2. 添加 CSV 输出和自动绘图；
3. 增加 median、p95、standard deviation；
4. 比较不同 CPU core 的结果；
5. 比较 SMT sibling、same L3、different L3；
6. 在 Intel i7-12700 lab machine 上复现实验；
7. 增加 TLB latency 实验；
8. 增加 huge page 对比；
9. 增加 page fault 实验；
10. 增加 scheduler migration 对 cache locality 的影响实验；
11. 在多 NUMA node 机器上使用 `numactl` 测 local/remote DRAM；
12. 查找 AMD 平台可用的 L3/LLC PMU events 或 IBS 采样方法。

---

# 12. 简短总结

`01_memory_hierarchy` 模块目前已经证明：

```text
L1/L2/L3/DRAM 的层级差异可以通过用户态程序观测到。
```

同时也说明：

```text
访问模式、数据布局、cache line、hardware prefetch、cache coherence 和 CPU topology
都会显著影响程序性能。
```