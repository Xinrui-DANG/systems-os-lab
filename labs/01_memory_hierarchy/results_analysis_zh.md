# 01 Memory Hierarchy 中文结果分析

本模块的目标不是单纯写几个 benchmark，而是用一组逐步递进的实验，把 CPU cache、cache line、hardware prefetch、false sharing、L3 cluster 这些概念和可观测的程序行为连接起来。

当前已完成的实验包括：

| 实验 | 核心问题 | 当前结论 |
|---|---|---|
| `cache_latency` | working set 变大后，访问延迟如何跨过 L1/L2/L3/DRAM 边界 | random pointer chasing 可以测出明显的缓存层级延迟阶梯 |
| `sequential_vs_random` | 为什么顺序访问远快于随机访问 | 顺序访问可被硬件预取和内存并行性优化，随机依赖访问暴露真实 load latency |
| `cache_line_effect` | stride 如何影响 cache line 利用率 | stride 越大，每条 64B cache line 中真正有用的数据越少 |
| `false_sharing` | 两个线程写不同变量为什么仍会互相拖慢 | cache coherence 以 cache line 为单位工作，不以变量为单位工作 |
| `prefetch_effect` | 软件预取和硬件预取分别什么时候有效 | 对简单顺序流，硬件预取已经很强，手动 prefetch 不一定更快 |
| `numa_or_l3_cluster` | 换 CPU 后是否出现 local/remote cache 差异 | 当前机器没有稳定证明跨 L3 cluster 更慢，结论是负/不确定结果 |

## 1. cache_latency：缓存层级延迟

`cache_latency` 使用 random pointer chasing：

```c
idx = next[idx];
```

这个访问模式的关键是：下一次访问的地址必须等当前 load 完成之后才知道。因此 CPU 很难提前预取，也很难并行发出多个独立内存请求。

这和顺序数组遍历不同。顺序访问的地址模式很容易被预测，而 pointer chasing 会把访问延迟暴露出来。

实验思想：

1. 构造一个随机排列。
2. 把数组下标连接成一个随机环。
3. 从 4 KiB 到 512 MiB 逐步增大 working set。
4. 每个 working set 运行固定次数的依赖 load。
5. 输出 `ns/access`。

这个实验回答的是：

```text
当 working set 从 L1 能装下，变成 L2/L3/DRAM 才能装下时，
一次随机依赖 load 的成本如何变化？
```

它的意义是建立 memory hierarchy 的第一条主线：**缓存层级不是抽象结构，而是能从用户态程序里测出来的延迟阶梯。**

## 2. sequential_vs_random：硬件预取的效果

这个实验对比两种访问模式：

```c
sum += data[idx];
idx = (idx + 1) & mask;
```

和：

```c
idx = next[idx];
```

sample run 中，顺序访问大约保持在 `0.38-0.45 ns/access`，而随机依赖访问在大 working set 下上升到 `100 ns/access` 左右。

这个差距说明：

- 顺序访问有稳定的地址流。
- CPU hardware prefetcher 可以提前把未来 cache line 拉进来。
- 顺序访问还可以利用 memory-level parallelism。
- random pointer chasing 的下一个地址不可提前知道，因此预取器很难帮忙。

这个实验不是在测 DRAM 最高带宽，也不是在测单次真实 DRAM latency。它展示的是：**同样是读内存，访问模式可以让 CPU 隐藏大量延迟，也可以迫使 CPU 暴露延迟。**

## 3. cache_line_effect：空间局部性和 cache line 利用率

`cache_line_effect` 使用不同 stride 读取同一个 256 MiB byte array：

```text
1B, 2B, 4B, 8B, 16B, 32B, 64B, 128B, 256B, 512B, 1024B, 4096B
```

sample run 中：

```text
stride 1B:   0.233 ns/access, line utilization 100.00%
stride 64B:  1.955 ns/access, line utilization   1.56%
stride 4096B: 6.278 ns/access, line utilization  1.56%
```

这个实验的核心是：CPU 从内存拉数据进 cache 时，通常不是只拉一个 byte，而是拉一整条 cache line，常见大小为 64B。

如果程序按 `stride = 1B` 访问，那么每条 cache line 的 64 个 byte 最终都可能被用到。

如果程序按 `stride = 64B` 访问，那么每次只用一条 cache line 里的 1 个 byte，其余 63 个 byte 都是被搬进来了但没被利用。

所以这个实验说明：

```text
cache miss 的成本不只取决于是否 miss，
还取决于 miss 之后搬进来的 cache line 有多少数据被真正利用。
```

这就是 spatial locality 的实际含义。

## 4. false_sharing：cache coherence 的粒度

`false_sharing` 对比两种数据布局。

第一种是两个 counter 靠在一起：

```c
typedef struct {
    volatile uint64_t a;
    volatile uint64_t b;
} shared_counters_t;
```

第二种是把每个 counter padding 到 64B：

```c
typedef struct __attribute__((aligned(64))) {
    volatile uint64_t value;
    char padding[64 - sizeof(uint64_t)];
} padded_counter_t;
```

两个线程分别写自己的 counter。逻辑上它们没有共享变量，但如果两个 counter 位于同一条 cache line，cache coherence 协议仍然会把整条 cache line 的 ownership 在 CPU 之间来回转移。

sample run 中，padding 后约 `6.75x` 加速。

这个实验说明：

```text
多线程程序里的共享，不只发生在变量层面，
也发生在 cache line 层面。
```

因此性能优化时，数据布局和 cache line 对齐很重要。尤其是高频写入的 per-thread counter、queue metadata、lock state 等结构，很容易出现 false sharing。

## 5. prefetch_effect：软件预取不一定更快

这个实验对比三种模式：

```c
sum += data[i];                         // sequential
__builtin_prefetch(&data[i + distance]); // software prefetch
idx = next[idx];                        // random chase
```

sample run：

```text
sequential          0.237 ns/access
software_prefetch   0.344 ns/access
random_chase      101.448 ns/access
```

结果看起来有点反直觉：手动 software prefetch 比普通 sequential 更慢。

原因是：

- 顺序访问本身已经非常容易被硬件预取器识别。
- `__builtin_prefetch` 额外增加了指令和前端压力。
- 如果 prefetch distance 不合适，可能太早、太晚，或者污染 cache。
- 对已经很规则的 streaming access，手写 prefetch 不一定比硬件更聪明。

而 random pointer chasing 依然很慢，因为下一个地址必须依赖当前 load 的结果。

所以这个实验的结论不是“software prefetch 没用”，而是：

```text
software prefetch 只在硬件预取器无法充分处理、
且程序能足够早知道未来地址时才可能有价值。
```

对于简单顺序流，硬件预取已经非常强。

## 6. numa_or_l3_cluster：负结果也是结果

这个实验最初想回答：

```text
如果数据在 CPU A 上 warm up，然后切到 CPU B 上访问，
是否会出现 local/remote cache 或 L3 cluster 差异？
```

程序做法是：

1. 构造随机 pointer-chasing cycle。
2. pin 到 warm CPU，先走一遍 pointer chain。
3. pin 到 measure CPU，再计时访问。
4. 对比 same CPU、same L3、other L3。

当前机器 topology：

```text
AMD Ryzen 9 8945HX
32 logical CPUs
16 physical cores
2 threads per core
1 NUMA node
2 L3 instances
```

关键 CPU placement：

| 关系 | warm CPU | measure CPU |
|---|---:|---:|
| same logical CPU | 0 | 0 |
| SMT sibling | 0 | 16 |
| different core, same L3 | 0 | 1 |
| different core, different L3 | 0 | 8 |

重复 7 次的 16 MiB 结果：

```text
same CPU:  13.113 ns/access median
same L3:   12.619 ns/access median
other L3:  12.979 ns/access median
```

这个结果没有稳定证明跨 L3 cluster 更慢。working-set sweep 也没有给出单调趋势。

这不是失败，而是一个重要结论：

```text
当前这个 benchmark 在这台机器上没有隔离出强 L3-cluster penalty。
```

可能原因包括：

- 机器只有 1 个 NUMA node，不是 remote NUMA DRAM 测试。
- 16 MiB working set 可能仍然适合当前 L3 结构。
- 64 MiB 时可能已经受 DRAM、频率和调度噪声主导。
- CPU frequency scaling 会影响短时间 benchmark。
- 单纯 timing 无法区分 L1/L2/L3/DRAM/频率变化。
- 跨 L3 的差异可能小于 run-to-run noise。

因此这个实验现在被定位为 topology probe，而不是完整证明。

后续需要：

- 固定频率或至少记录频率。
- 运行更多 repeated trials。
- 使用 perf counters。
- 在多 NUMA node 机器上用 `numactl` 控制 memory placement。
- 分别测试 SMT sibling、same L3、different L3、different NUMA node。

## 总体结论

这一组实验可以串成一条完整逻辑链：

1. `cache_latency` 说明随机依赖 load 会暴露缓存层级延迟。
2. `sequential_vs_random` 说明访问模式决定 CPU 能否隐藏延迟。
3. `cache_line_effect` 说明 cache line 的利用率决定空间局部性收益。
4. `false_sharing` 说明 cache coherence 的真实粒度是 cache line。
5. `prefetch_effect` 说明硬件预取对规则访问很强，手动预取不是万能加速器。
6. `numa_or_l3_cluster` 说明 topology 实验必须结合 CPU 拓扑、重复运行和硬件计数器，不能只凭一次 timing 下结论。

从 OS 和系统角度看，这些实验共同说明：

```text
程序性能不是只由算法复杂度决定，
还由地址模式、数据布局、缓存层级、CPU 拓扑、调度位置和内存放置共同决定。
```

这也是后续 virtual memory、scheduler affinity、synchronization、RTOS jitter、runtime isolation 实验的基础。
