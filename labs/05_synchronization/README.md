# 05 Synchronization

## Goal

Study mutexes, spinlocks, atomics, condition variables, producer-consumer queues, and contention.

## Core Questions

1. What is the cost of mutual exclusion?
2. When does spinning waste CPU?
3. How do atomics behave under contention?
4. How do synchronization primitives affect cache coherence traffic?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `mutex_counter.c` | What does lock contention cost? | mutex |
| `spinlock_counter.c` | When is spinning bad? | spinlock |
| `atomic_counter.c` | What does atomic contention cost? | atomic RMW |
| `producer_consumer.c` | How do threads coordinate? | condition variable |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- race condition
- mutual exclusion
- lock contention
- atomic operation
- memory ordering
- cache coherence

## Connection to Modern Systems

Synchronization affects real-time blocking, priority inversion, shared-memory IPC, driver queues, and accelerator runtime command submission.
