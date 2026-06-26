# 04 Syscall / Interrupt

## Status

Scaffolded. Experiments are designed but not implemented yet.

## Goal

Study the boundary between user space and kernel space through syscalls, signals, timers, and interrupt-like wakeup behavior.

## Core Questions

1. What does a syscall cost?
2. How are signals delivered?
3. How accurate are timers?
4. What wakeup latency does the kernel introduce?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `syscall_cost.c` | What does entering the kernel cost? | syscall boundary |
| `signal_delivery.c` | How are signals delivered? | signal handling |
| `timerfd_latency.c` | How accurate is timerfd? | kernel timers |
| `nanosleep_jitter.c` | How much wakeup jitter appears? | scheduler wakeup |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

See `analysis_zh.md`.

## Key Concepts

- user space
- kernel space
- syscall
- trap
- interrupt
- signal
- timer
- wakeup latency

## Connection to Modern Systems

Syscalls, timer interrupts, and wakeup latency are core mechanisms for RTOS behavior, Linux scheduling, driver interactions, and hypervisor exits.
