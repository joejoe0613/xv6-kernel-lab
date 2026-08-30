# xv6 Kernel Lab

A hands-on operating system kernel project based on
[MIT xv6-RISC-V](https://github.com/mit-pdos/xv6-riscv).

This project was created to bridge the gap between operating system theory
and practical kernel implementation.

The work focuses on system calls, process scheduling, starvation prevention,
process inspection, synchronization, and inter-process communication.

---

## Project Overview

During this project, I extended xv6-RISC-V with several kernel-level features
and user-space test programs.

The implementation was developed incrementally over eight weeks.

---

## Week 1 — xv6 Environment and User Programs

- Set up the RISC-V GCC toolchain and QEMU on Linux Mint
- Built and booted xv6-RISC-V
- Studied the basic xv6 source tree
- Added custom user-space programs to xv6

The main goal of this stage was to understand the relationship between:

```text
Host Linux System
      |
      v
QEMU RISC-V Emulator
      |
      v
xv6 Operating System
```

---

## Week 2 — Custom System Calls

- Implemented a simple custom system call
- Implemented a system call with arguments and a return value
- Studied the path from user space to kernel space
- Learned how xv6 system call stubs are generated through `usys.pl`

Example system call flow:

```text
User Program
    |
    v
System Call Stub
    |
    v
ecall
    |
    v
Kernel Trap Handler
    |
    v
syscall()
    |
    v
sys_xxx()
```

Example custom system calls implemented during the exercise included:

```c
hello();
add(int a, int b);
```

---

## Week 3 — System Call Tracing and Context Switching

- Added basic system call tracing
- Observed process scheduling behavior
- Studied `scheduler()`, `yield()`, and `sched()`
- Examined context switching through `swtch.S`
- Used parent and child processes to observe process execution interleaving

The basic scheduling flow studied in this stage was:

```text
RUNNABLE Process
      |
      v
scheduler()
      |
      v
RUNNING
      |
      v
swtch()
      |
      v
Process Execution
```

The context switch mechanism was examined through the RISC-V register
save and restore operations in `swtch.S`.

---

## Week 4 — Priority Scheduler

The default scheduling behavior was extended with process priorities.

Implemented features:

- Added a priority field to `struct proc`
- Initialized process priorities during process creation
- Implemented a priority-based scheduling policy
- Added a `setpriority()` system call
- Developed user-space test programs for different priorities

In this implementation:

```text
Smaller priority value = Higher scheduling priority
```

For example:

```text
Priority 1  -> High priority
Priority 10 -> Medium priority
Priority 20 -> Low priority
```

The scheduler searches for RUNNABLE processes and gives preference to
the process with the highest scheduling priority.

---

## Week 5 — Aging and Starvation Prevention

Strict priority scheduling can cause low-priority processes to experience
starvation.

To address this problem, an aging mechanism was added.

Implemented features:

- Added process waiting-time tracking
- Recorded how long RUNNABLE processes waited for CPU time
- Increased the effective priority of processes that waited too long
- Developed an aging test program
- Observed starvation and fairness behavior

Conceptually:

```text
Low Priority Process
        |
        v
Waits for CPU
        |
        v
Waiting Time Increases
        |
        v
Aging Improves Priority
        |
        v
Higher Chance of Scheduling
```

This experiment demonstrated the trade-off between strict priority scheduling
and scheduling fairness.

---

## Week 6 — Process Information and ps-like Command

A kernel-to-user process information interface was implemented.

Features include:

- Added a `getpinfo()` system call
- Collected process information from the kernel process table
- Used `copyout()` to safely transfer kernel data to user space
- Implemented a `ps`-like user command
- Added process information test programs

The exported process information includes:

```text
PID
STATE
PRIORITY
WAIT TIME
PROCESS NAME
```

Example output:

```text
PID  STATE       PRI  WAIT  NAME
--------------------------------
1    SLEEPING    10   0     init
2    SLEEPING    10   0     sh
3    RUNNING     5    0     ps
```

This exercise provided practical experience with the separation between
kernel memory and user address spaces.

The basic data transfer flow is:

```text
Kernel Process Table
        |
        v
getpinfo()
        |
        v
copyout()
        |
        v
User Address Space
        |
        v
ps Command
```

---

## Week 7 — Kernel Semaphore and Process Synchronization

A blocking kernel semaphore was implemented.

Supported operations:

```c
sem_create(int initial_value);
sem_wait(int semaphore_id);
sem_post(int semaphore_id);
```

The semaphore implementation uses:

- Kernel spinlocks
- `sleep()`
- `wakeup()`
- Blocking synchronization
- Process state transitions

Unlike busy waiting, a process waiting for an unavailable semaphore enters
the `SLEEPING` state so the CPU can execute another process.

Example synchronization flow:

```text
Process A
   |
   v
sem_wait()
   |
   v
Semaphore unavailable
   |
   v
SLEEPING


Process B
   |
   v
sem_post()
   |
   v
wakeup()
   |
   v
Process A becomes RUNNABLE
```

Two types of semaphore behavior were tested.

### Blocking behavior

```text
Child
  |
sem_wait()
  |
SLEEPING
  |
  |       Parent
  |         |
  |      sem_post()
  |         |
  +---------+
      wakeup
        |
        v
Child resumes execution
```

### Stored semaphore signal

If `sem_post()` occurs before `sem_wait()`, the semaphore value records the
available resource so that the later `sem_wait()` does not need to block.

This demonstrated both counting semaphore behavior and blocking
synchronization.

---

## Week 8 — Blocking Kernel Message Queue

A kernel-level blocking message queue based on a circular buffer was
implemented.

Supported operations:

```c
qcreate();
qsend(int queue_id, int value);
qrecv(int queue_id, int *value);
```

The queue implementation combines:

- Circular buffers
- Kernel spinlocks
- `sleep()` and `wakeup()`
- Blocking producer-consumer synchronization
- Kernel-to-user data transfer
- Inter-process communication

The queue uses:

```text
head  -> Next item to remove
tail  -> Next position to insert
count -> Number of stored items
```

Example circular buffer:

```text
+----+----+----+----+
| 0  | 1  | 2  | 3  |
+----+----+----+----+
  ^
 head

tail wraps back to index 0
after reaching the end.
```

### Producer Behavior

When the queue still has free space:

```text
Producer
   |
   v
qsend()
   |
   v
Insert Data
```

When the queue becomes full:

```text
Producer
   |
   v
qsend()
   |
   v
Queue Full
   |
   v
SLEEPING
```

### Consumer Behavior

When data is available:

```text
Consumer
   |
   v
qrecv()
   |
   v
Remove Data
```

When the queue is empty:

```text
Consumer
   |
   v
qrecv()
   |
   v
Queue Empty
   |
   v
SLEEPING
```

When the consumer removes an item from a full queue:

```text
Consumer removes item
        |
        v
Queue has free space
        |
        v
Wake Producer
```

When the producer inserts an item into an empty queue:

```text
Producer inserts item
        |
        v
Queue contains data
        |
        v
Wake Consumer
```

Example producer-consumer test result:

```text
producer sending 0
producer sending 1
producer sending 2
producer sending 3

consumer received 0
consumer received 1
consumer received 2
consumer received 3

producer sending 4
producer sending 5
producer sending 6
producer sending 7

consumer received 4
consumer received 5
consumer received 6
consumer received 7

producer sending 8
producer sending 9

consumer received 8
consumer received 9

queue test finished
```

This experiment demonstrated blocking IPC and producer-consumer
synchronization inside the xv6 kernel.

---

# Implemented Features

## Kernel Extensions

- Custom system calls
- System call argument passing
- System call tracing
- Priority-based scheduling
- Aging mechanism
- Starvation prevention
- Process priority management
- Process information interface
- Kernel semaphore
- Blocking synchronization
- Kernel message queue
- Producer-consumer IPC

---

## User Programs and Tests

Test programs developed during the project include:

```text
hello
testhello
testadd
yieldtest
prioritytest
agingtest
ps
pinfotest
semtest
semsignal
queuetest
```

Some test programs may exist only in specific development branches depending
on the stage of the project.

---

# Key Concepts Practiced

This project provided hands-on experience with:

- User mode and kernel mode
- RISC-V system calls
- `ecall`
- Trap handling
- Process states
- Process scheduling
- Context switching
- RISC-V register context
- Priority scheduling
- Starvation
- Aging
- Scheduling fairness
- Kernel synchronization
- Spinlocks
- Semaphores
- Blocking synchronization
- `sleep()`
- `wakeup()`
- Race conditions
- Critical sections
- Kernel/user memory separation
- `copyout()`
- Producer-consumer synchronization
- Circular buffers
- Inter-process communication

---

# Project Architecture

```text
                  User Programs
                       |
                       v
              System Call Interface
                       |
                       v
        +-----------------------------+
        |         xv6 Kernel          |
        |                             |
        |  System Call Handling       |
        |                             |
        |  Process Management         |
        |        |                    |
        |        v                    |
        |  Priority Scheduler         |
        |        |                    |
        |        v                    |
        |      Aging                  |
        |                             |
        |  Process Inspection         |
        |                             |
        |  Kernel Semaphore           |
        |                             |
        |  Blocking Message Queue     |
        |                             |
        +-----------------------------+
                       |
                       v
                  RISC-V ISA
                       |
                       v
                      QEMU
```

---

# Development Environment

The project was developed and tested using:

- Linux Mint
- MIT xv6-RISC-V
- QEMU RISC-V emulator
- RISC-V GNU Compiler Toolchain
- GCC
- GNU Make
- Git

---

# Building and Running

Install the required RISC-V toolchain and QEMU before building xv6.

From the project root directory:

```bash
make clean
make qemu CPUS=1
```

The project was primarily tested with one simulated CPU while developing and
debugging scheduling and synchronization mechanisms.

After xv6 boots successfully:

```text
xv6 kernel is booting

init: starting sh
$
```

---

# Example Tests

## Custom System Call

```text
testhello
```

or:

```text
testadd
```

---

## Scheduler Observation

```text
yieldtest
```

---

## Priority Scheduler

```text
prioritytest
```

---

## Aging Scheduler

```text
agingtest
```

---

## Process Information

```text
ps
```

or:

```text
pinfotest
```

---

## Kernel Semaphore

```text
semtest
```

Blocking event synchronization:

```text
semsignal
```

---

## Producer-Consumer Message Queue

```text
queuetest
```

---

# Learning Motivation

My operating systems coursework primarily focused on conceptual topics such as:

- Processes
- Threads
- Scheduling
- Synchronization
- Deadlocks
- Memory management
- Virtual memory
- File systems

The course provided the theoretical foundation, but included limited
kernel-level programming experience.

This project was developed as a hands-on extension to connect operating system
theory with practical kernel implementation.

The longer-term goal is to build stronger foundations for:

- Embedded software
- Firmware development
- Real-time operating systems
- Low-level system programming

---

# What I Learned

Through this project, I gained a clearer understanding of how operating system
concepts are implemented in real kernel code.

In particular, the project helped connect concepts such as:

```text
Process Scheduling
        |
        v
Context Switching
```

```text
Critical Section
        |
        v
Spinlock / Semaphore
```

```text
Blocking Process
        |
        v
sleep() / wakeup()
```

```text
Kernel Data
        |
        v
copyout()
        |
        v
User Program
```

```text
Producer / Consumer
        |
        v
Blocking Message Queue
```

These experiments helped transform operating system concepts from textbook
knowledge into practical implementation experience.

---

# Future Work

Possible future extensions include:

- Multi-core scheduler testing
- Round-robin fairness improvements
- More advanced scheduling policies
- Priority inheritance
- Priority inversion experiments
- Dynamic semaphore destruction
- Dynamic message queue allocation
- Queue termination and sentinel messages
- Memory management experiments
- Page table experiments
- Page replacement algorithms
- Copy-on-write experiments
- File system experiments
- FreeRTOS task scheduling
- FreeRTOS queues and semaphores
- FreeRTOS mutexes
- Interrupt-to-task synchronization
- Priority inversion in RTOS
- Lightweight RTOS implementation

---

# Base Project

This project is based on the MIT PDOS xv6-RISC-V project:

https://github.com/mit-pdos/xv6-riscv

xv6 is a re-implementation of Unix Version 6 for educational purposes.

The original xv6 source code, copyright notices, and licensing information
belong to their respective authors.

This repository contains educational modifications and extensions developed
for operating system study and experimentation.

The original upstream README and licensing information are preserved in this
repository.
