# 08 Real-Time Interference

## Status

Scaffolded. Experiments are designed but not implemented yet.

## Goal

Measure how periodic task latency and jitter change under CPU, memory, cache, and I/O interference.

Long-term, this module provides the timing-analysis side of GPU/NPU interference studies: average latency is not enough; the key question is whether worst-case response time can remain within a deadline.

## Core Questions

1. What is the baseline latency of a periodic task?
2. How does CPU stress change wakeup latency?
3. How does memory/cache stress change jitter?
4. How should WCRT and deadline misses be reported?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `periodic_task_latency.c` | What is baseline wakeup latency? | periodic task |
| `cpu_stress_interference.c` | How does CPU load affect latency? | CPU contention |
| `memory_stress_interference.c` | How does memory pressure affect latency? | bandwidth contention |
| `cache_stress_interference.c` | How does cache pressure affect latency? | cache interference |
| `io_stress_interference.c` | How does I/O affect wakeups? | kernel and storage activity |
| `latency_histogram.py` | How is latency distributed? | histogram |
| `wcrt_analysis.py` | What is the worst observed response time? | WCRT analysis |

## Long-Term Direction

Future accelerator-aware analysis should model a task as:

```text
Task_i =
  CPU segment
  + DMA/copy segment
  + GPU/NPU compute segment
  + blocking
  + interference
```

The long-term question is:

```text
R_i <= D_i ?
```

where `R_i` is response time and `D_i` is the deadline.

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- periodic task
- scheduling latency
- jitter
- p99 latency
- deadline miss
- WCRT
- WCET
- interference

## Connection to Modern Systems

This module maps directly to RTOS/QNX/Linux safety-domain jitter, untrusted AI workload interference, and WCRT/WCET degradation on heterogeneous platforms.
