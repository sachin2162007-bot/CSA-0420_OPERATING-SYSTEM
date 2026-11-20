# Comprehensive Operating Systems Architecture Guide

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assessment 2 (CO2)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  

---

## 1. Process Management & Execution Models

### 1.1 Process Lifecycle & The Five-State Model
An operating system process is an active program in execution containing program code, current activity represented by the program counter and CPU registers, and an associated address space (stack, heap, data, text).

```
   [ NEW ] ---> (Admitted) ---> [ READY ] <--- (Interrupt / Quantum Expired) --- [ RUNNING ] ---> (Exit) ---> [ TERMINATED ]
                                   ^                                                |
                                   |                                                v
                                   +--- (I/O or Event Completion) <--- [ WAITING ] <+ (I/O or Event Wait)
```

- **New:** Process is constructed; PCB initialized in kernel slab memory.
- **Ready:** Placed in the scheduler run-queue, ready for hardware dispatch.
- **Running:** Active hardware instruction execution on a CPU core.
- **Waiting:** Suspended awaiting asynchronous I/O completion, hardware interrupts, or synchronization locks.
- **Terminated:** Execution ended; resources deallocated; PCB preserved as zombie until reaped by parent `wait()`.

### 1.2 The Process Control Block (PCB) Architecture
Linux represents the PCB using `struct task_struct`, containing:
1. `pid_t pid, tgid`: Unique process identifier and thread group identifier.
2. `volatile long state`: Execution state flags (`TASK_RUNNING`, `TASK_INTERRUPTIBLE`, `TASK_UNINTERRUPTIBLE`, `EXIT_ZOMBIE`).
3. `struct thread_struct thread`: CPU-specific hardware registers saved across context switches (RSP, RIP, FS, GS, CR2, CR3).
4. `struct mm_struct *mm`: Memory management descriptor pointing to page tables (`pgd_t *pgd`) and virtual memory areas.
5. `struct files_struct *files`: File descriptor array mapping integer handles (`fd 0, 1, 2`) to open file descriptions.
6. `struct signal_struct *signal`: Pending and blocked signal bitmasks.

---

## 2. CPU Scheduling Architecture

### 2.1 Scheduling Criteria Formulas
- **Turnaround Time ($TAT$):** Interval from arrival time ($AT$) to completion time ($CT$):
  $$TAT = CT - AT$$
- **Waiting Time ($WT$):** Total duration spent waiting in the ready queue:
  $$WT = TAT - BT$$
- **Response Time ($RT$):** Time elapsed from arrival until the first time the process receives CPU execution:
  $$RT = \text{First CPU Time} - AT$$

### 2.2 Algorithm Classification
1. **First-Come, First-Served (FCFS):** Non-preemptive FIFO queue. Vulnerable to the **Convoy Effect**, where short jobs wait behind massive compute-bound jobs.
2. **Shortest Job First (SJF):** Non-preemptive optimal policy for minimizing average waiting time. Unfeasible in practice because future CPU burst lengths cannot be known in advance.
3. **Shortest Remaining Time First (SRTF):** Preemptive variant of SJF. Preempts running process if a newly arrived process possesses a shorter remaining burst.
4. **Priority Scheduling:** Dispatches the process with highest priority (lowest integer value). Vulnerable to **indefinite blocking (starvation)**. Mitigated via **Aging**, which progressively boosts the priority of waiting jobs.
5. **Round Robin (RR):** Preemptive scheduling where each process receives a discrete time slice (quantum $q$). Ensures fairness and bounded response time. If $q$ is too small, context-switch overhead dominates; if $q$ is too large, it degenerates into FCFS.
6. **Multilevel Feedback Queue (MLFQ):** Dynamically categorizes jobs into multiple priority queues with increasing quanta based on observed CPU-burst behavior.

---

## 3. Multithreading Architecture & Concurrency

### 3.1 Memory Layout
Within a single process:
- **Shared Resources:** Text/Code, Data/BSS, Heap, File Descriptors, Sockets, Signals.
- **Private Resources:** Program Counter (PC), CPU Registers, Execution Call Stack (Local variables, frame pointers).

### 3.2 Thread Models
- **Many-to-One:** User-level runtime manages threads. Fast switching, but blocking I/O halts the entire process; no multi-core hardware scaling.
- **One-to-One (Linux NPTL):** Each user thread maps to a kernel `task_struct`. True parallel execution across multi-core CPUs.
- **Many-to-Many:** Multiplexes $M$ user threads onto $N$ kernel threads, balancing dispatch speed and multi-core utilization.

---

## 4. Inter-Process Communication (IPC)

### 4.1 Message Passing vs Shared Memory
- **UNIX Pipes:** Kernel-buffered unidirectional FIFO. Requires two memory copies (User $\rightarrow$ Kernel buffer on write, Kernel buffer $\rightarrow$ User on read). Automatic kernel synchronization.
- **POSIX Shared Memory:** Direct mapping of physical memory frames into multiple address spaces (`mmap`). Zero kernel copies. Bounded only by RAM bandwidth ($> 3.7\,\text{GB/s}$). Requires explicit semaphore synchronization.
- **POSIX Message Queues:** Structured messages with priority ordering and message boundary preservation.

---

## 5. Synchronisation & The Critical Section Problem

### 5.1 The Three Mandatory Conditions
1. **Mutual Exclusion:** Exactly one thread can enter the critical section at a time.
2. **Progress:** Only threads not in their remainder section decide who enters next; selection cannot be postponed indefinitely.
3. **Bounded Waiting:** A bound exists on the number of entries granted to peers before a waiting thread enters (no starvation).

### 5.2 Semaphores vs Mutexes vs Monitors
- **Binary Semaphore / Mutex:** Enforces mutual exclusion ($S \in \{0, 1\}$).
- **Counting Semaphore:** Controls access to a finite resource pool ($S \ge 0$).
- **Monitor:** High-level language construct combining mutual exclusion with condition variables (`wait()`, `signal()`), eliminating manual semaphore pairing bugs.

---

## 6. Deadlock Handling & Banker's Algorithm

### 6.1 The Four Coffman Conditions
1. Mutual Exclusion
2. Hold and Wait
3. No Preemption
4. Circular Wait

### 6.2 Banker's Algorithm Complexity
- **Safety Algorithm:** $O(m \cdot n^2)$ time complexity, where $n$ is process count and $m$ is resource count.
- **Resource-Request Algorithm:** Verifies request $\le$ Need and request $\le$ Available, simulates trial allocation, and runs the safety algorithm.
