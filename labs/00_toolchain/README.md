# 00 Toolchain

## Status

Baseline implementation. It provides the measurement-environment checks used before interpreting later experiments.

## Goal

Set up and verify the measurement environment used by the rest of this repository.

This module does not study one OS mechanism directly. Instead, it records the compiler, CPU topology, cache hierarchy, available performance counters, CPU affinity behavior, and basic timing assumptions used by later experiments.

本模块不是研究某一个具体 OS 机制，而是为后续实验建立测量基准：编译器、CPU 拓扑、cache 层级、perf counter、CPU 亲和性、计时方式等。

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

## Run

```bash
make run
```

Collect machine profile:

```bash
make profile
```

Probe perf events:

```bash
make perf
```

## Results

Generated outputs are stored in `results/`.

## Analysis

See [analysis_zh.md](analysis_zh.md).

## Key Concepts

- compiler flags
- CPU affinity
- perf events
- sysfs
- procfs
- cache topology
- performance counters
- reproducibility
- measurement baseline

## Connection to Modern Systems

Measurement quality determines whether later claims about cache, TLB, scheduling latency, and interference are trustworthy.

For real-time and heterogeneous systems, a result without machine profile, CPU placement, compiler flags, and timing method is usually not interpretable.
