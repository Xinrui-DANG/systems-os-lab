# 08 Real-Time Interference

## Status

Scaffolded. Experiments are designed but not implemented yet.

## Goal

Measure how periodic task latency and jitter change under CPU, memory, cache, and I/O interference.

Long-term accelerator-aware timing questions are tracked separately in [../../long-term-plan/](../../long-term-plan/). This module focuses on measurable CPU-side latency and interference first.

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

See [../../long-term-plan/research_questions.md](../../long-term-plan/research_questions.md) for the future response-time questions that build on this module.

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

This module maps directly to RTOS/QNX/Linux safety-domain jitter and WCRT/WCET-style latency reporting.
