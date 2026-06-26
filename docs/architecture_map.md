# Architecture Map

```text
Application Behavior
  -> Access Pattern
  -> Virtual Address
  -> Page Table / TLB
  -> Physical Page
  -> Cache / DRAM
  -> Process Boundary
  -> Syscall / Interrupt
  -> Scheduler Decision
  -> Timing / Jitter
  -> Interference
  -> Isolation Boundary
  -> Device / Accelerator Runtime
```

This repository is organized so that each lab connects one piece of that path to measurable behavior.

## Three Layers

```text
Layer 1: Observable Labs
  -> user-space experiments that expose OS and hardware behavior

Layer 2: Mini-Kernel
  -> long-term Unix-like kernel mechanisms in QEMU

Layer 3: Systems Direction
  -> real-time interference, runtime isolation, IOMMU, hypervisor, heterogeneous runtime
```

## Direction

```text
classic OS mechanisms
  -> timing and interference
  -> isolation
  -> heterogeneous runtime behavior
```
