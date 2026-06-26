# 03 Process / Thread / Scheduler

## Goal

Study process creation, thread creation, context switching, scheduling latency, priority inversion, and scheduler policy behavior.

## Core Questions

1. How are processes created and replaced with new program images?
2. What does a context switch cost?
3. How expensive is thread creation?
4. How much scheduling latency does Linux introduce?
5. How can priority inversion affect real-time behavior?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `fork_exec_demo.c` | How do `fork`, `exec`, and `wait` compose? | process creation |
| `context_switch_cost.c` | What is context-switch overhead? | pipe ping-pong |
| `pthread_create_cost.c` | How expensive is thread creation? | pthread |
| `scheduler_latency.c` | How late does a periodic task wake up? | scheduler latency |
| `priority_inversion.c` | How can a low-priority task block a high-priority task? | priority inversion |
| `mini_scheduler_sim.c` | How do FIFO/RR/Priority/EDF differ? | scheduler simulation |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- process
- thread
- context switch
- preemption
- scheduling latency
- jitter
- deadline miss
- priority inversion
- real-time task

## Connection to Modern Systems

Scheduler behavior connects directly to RTOS/QNX/Linux coexistence, WCRT analysis, interference control, and mixed-criticality systems.
