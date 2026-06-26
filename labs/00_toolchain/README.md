# 00 Toolchain

## Goal

Set up the measurement tools used by the rest of the repository.

## Core Questions

1. What compiler and flags are used?
2. How do we pin work to a CPU?
3. How do we inspect CPU/cache topology?
4. Which perf events are available on this machine?

## Experiments

| Experiment | Question | Mechanism |
|---|---|---|
| `collect_machine_profile.sh` | What machine is this result from? | `lscpu`, `/proc`, sysfs |
| `perf_event_probe.sh` | Which counters are available? | Linux perf |
| `cpu_affinity_check.c` | Can this process pin to a CPU? | scheduler affinity |

## Build

```bash
make
```

## Results

See `results/`.

## Analysis

Use this module to document machine-specific assumptions.

## Key Concepts

- compiler flags
- CPU affinity
- perf events
- sysfs
- procfs

## Connection to Modern Systems

Measurement quality determines whether later claims about cache, TLB, scheduling latency, and interference are trustworthy.
