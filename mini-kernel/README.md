# Mini-Kernel

## Goal

A small Unix-like kernel for long-term learning.

## Target Features

- boot on QEMU
- timer interrupt
- preemptive scheduling
- virtual memory
- user/kernel separation
- basic syscalls
- simple device driver
- ramfs or tiny file system
- minimal shell

## Non-Goals

- full POSIX
- ext4
- TCP/IP stack
- USB stack
- SMP
- dynamic linker
- GPU/NPU driver
- production-level security

## Structure

```text
boot/
arch/
kernel/
mm/
sched/
syscall/
drivers/
user/
```

## Connection to Modern Systems

The mini-kernel is the place to combine mechanisms after the user-space labs have made them observable.
