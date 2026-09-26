# CSC 501 Operating Systems

Operating systems programming assignments completed for CSC 501 at North Carolina State University using the Xinu teaching operating system.

The work explores process execution, scheduling, synchronization, and virtual memory by modifying and extending kernel code in C.

## Lab overview

| Lab | Focus | Selected work |
| --- | --- | --- |
| Lab 0 | Kernel instrumentation | Process stack inspection, stack traces, segment addresses, and system call summaries |
| Lab 1 | CPU scheduling | Aging-based scheduling, a Linux-like scheduler, epoch accounting, and scheduler comparison |
| Lab 2 | Synchronization | Reader-writer locks, priority inheritance, priority inversion experiments, and lock lifecycle management |
| Lab 3 | Virtual memory | Backing stores, page directories and tables, page-fault handling, virtual heaps, and replacement policies |

`Extra_credits/` contains paper reviews covering scheduling, virtualization, file systems, resource containers, and I/O design.

## Repository structure

```text
csc501-lab0/       Kernel instrumentation and stack analysis
csc501-lab1/       Scheduling policies
csc501-lab2-qemu/  Locks and process synchronization
csc501-lab3/       Demand paging and virtual memory
Extra_credits/     Systems paper reviews
```

Each lab contains a complete Xinu tree together with its assignment specification and written analysis.

## Environment

- C
- Xinu
- x86 architecture
- GCC and Make
- QEMU for the later labs

These assignments were built for the course environment. A modern machine may require an older 32-bit toolchain or a compatible container or virtual machine.

## Building a lab

Build commands and runtime details vary by assignment. Start with the PDF in the selected lab directory. The usual workflow is:

```bash
cd csc501-lab1/compile
make clean
make
```

Run the resulting image using the emulator or course environment described in that lab's specification.

## What I learned

- How a small kernel represents processes, stacks, queues, and system calls
- How scheduling policy changes fairness, response time, and starvation behavior
- Why priority inversion occurs and how priority inheritance changes execution order
- How page faults connect virtual addresses to page tables, frames, and backing stores
- How to debug behavior that spans kernel data structures and hardware-level state

## Academic context

This repository documents completed coursework and is intended as a record of systems learning. Current students should follow their institution's academic integrity rules and should not submit this work as their own.

