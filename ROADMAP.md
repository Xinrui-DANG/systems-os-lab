# Roadmap

## Phase 0: Environment

- gcc, make, perf, taskset, lscpu
- sysfs and procfs inspection
- machine profile collection

## Phase 1: Memory Hierarchy

- cache latency
- sequential vs random access
- cache line effect
- false sharing
- prefetch effect
- NUMA or L3 cluster behavior

## Phase 2: Virtual Memory

- address layout
- page fault
- mmap
- TLB latency
- huge pages
- copy-on-write

## Phase 3: Process and Scheduler

- fork / exec / wait
- process tree
- thread creation
- context switch
- CPU time vs wall time
- CPU affinity and scheduler migration

## Phase 4: Synchronization and IPC

- mutex, spinlock, and atomic counters
- producer-consumer
- condition variables
- pipes, FIFO, Unix domain sockets
- shared memory, message queues, signals

## Phase 5: Filesystem and Kernel Boundary

- buffered I/O and page cache
- mmap file I/O
- fsync cost
- direct I/O
- procfs and sysfs
- character device and ioctl demo

## Phase 6: RTOS and Isolation

- FreeRTOS task scheduling
- priority inversion
- deadline miss
- timer jitter
- queue latency
- TMR voter
- Linux namespaces
- cgroups
- seccomp
- QEMU/KVM
- Jailhouse and IOMMU notes
- interference benchmarks
