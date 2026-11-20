# Operating Systems Assessment 2 — Viva Questions & Model Answers

**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Course:** CSA04 — Operating Systems  

---

### Question 1: What are the five states in the canonical process lifecycle model?
**Answer:** The five states are:
1. **New:** Process is being created and its PCB initialized.
2. **Ready:** Process has all resources and waits in the run-queue for CPU dispatch.
3. **Running:** Instructions are executing on a physical CPU core.
4. **Waiting (Blocked):** Process is waiting for an I/O event, signal, or lock.
5. **Terminated:** Execution has completed; PCB is retained as a zombie until reaped by parent `wait()`.

---

### Question 2: What is a Process Control Block (PCB) and what does it contain?
**Answer:** A PCB (`struct task_struct` in Linux) is the kernel's core data structure representing an active process. It stores the PID, PPID, process state, program counter, CPU register context, memory management tables (page directory pointers), list of open file descriptors, CPU scheduling priorities, and accounting information.

---

### Question 3: Why is creating 2,000 separate processes for student sessions an unsound architecture?
**Answer:** 
1. **Memory Exhaustion:** 2,000 processes require independent page tables, kernel stacks, and address spaces ($12\text{--}24\,\text{GB}$ RAM).
2. **Context-Switch Thrashing:** 2,000 processes yield the CPU frequently during typing/reading, generating tens of thousands of context switches per second and wasting over 45% of CPU cycles on TLB flushes and cache invalidation.
3. **I/O Blocking:** Synchronous disk writes for auto-saving answers saturate I/O queues, causing processes to hang.

---

### Question 4: How does a thread-based architecture differ from a process-based architecture?
**Answer:** Threads share the same virtual address space, heap, global variables, and open files of their parent process. Each thread maintains only a private program counter, register set, and execution stack. A thread switch does not reload page tables (`CR3`) or invalidate the TLB, making it 15–25x faster than a process context switch ($50\text{--}100\,\text{ns}$ vs $1.5\text{--}3\,\mu\text{s}$).

---

### Question 5: What is the primary drawback of using threads for student sessions?
**Answer:** **Zero fault isolation.** Because all threads share the same address space, if one student session triggers a segmentation fault (e.g. null pointer dereference), the OS sends `SIGSEGV` to the entire process, crashing all 2,000 student sessions instantly.

---

### Question 6: What was the result of your Round Robin implementation with quantum = 3?
**Answer:** The program executed with exact 100% fidelity to the assignment PDF:
- Gantt Chart: `P1(0-3) P2(3-6) P3(6-9) P4(9-12) P1(12-15) P2(15-16) P3(16-19) P4(19-21) P1(21-23) P3(23-26)`
- Turnaround Times: P1=23, P2=15, P3=24, P4=18 $\rightarrow$ **Average TAT = 20.00**
- Waiting Times: P1=15, P2=11, P3=15, P4=13 $\rightarrow$ **Average WT = 13.50**

---

### Question 7: How does time quantum size affect Round Robin performance?
**Answer:** 
- If quantum $q$ is very small ($q=1$), response time is optimal, but context-switch overhead skyrockets (25 context switches).
- If quantum $q$ is very large ($q=20$), Round Robin degenerates into FCFS (3 context switches), but short interactive jobs experience severe queuing delays behind large compute jobs.

---

### Question 8: Which scheduling algorithm achieved the lowest average turnaround time?
**Answer:** **Shortest Remaining Time First (SRTF)** achieved the optimal average turnaround time of **13.00** and average waiting time of **6.50**, because it preempts long jobs as soon as a shorter job arrives.

---

### Question 9: What is starvation in CPU scheduling and how does Aging solve it?
**Answer:** Starvation occurs in SJF or Priority scheduling when low-priority or long jobs remain in the ready queue indefinitely due to a continuous influx of higher-priority or shorter jobs. **Aging** resolves starvation by gradually increasing the priority of waiting jobs as time elapses, guaranteeing they eventually reach the front of the queue.

---

### Question 10: How does a Multilevel Feedback Queue (MLFQ) work?
**Answer:** MLFQ uses multiple priority queues with increasing time quanta. New jobs enter the top queue with a short quantum. If a job exhausts its time quantum without yielding, it is demoted to a lower queue with a larger quantum. Compute-heavy jobs sink to the bottom, while interactive I/O jobs remain at the top. Periodic priority boosts prevent starvation.

---

### Question 11: What is the dispatch time complexity of MLFQ compared to SJF?
**Answer:** MLFQ dispatches in **$O(1)$ constant time** using simple FIFO head/tail pointer operations on each queue. In contrast, SJF requires scanning or sorting the ready queue, costing **$O(n)$** per decision.

---

### Question 12: Why did thread speedup plateau at 8 threads in Task 03?
**Answer:** On an 8-core CPU, the hardware has 8 physical execution units. Moving from 8 to 16 threads caused execution time to increase (0.6920s to 0.7350s) due to CPU context-switch thrashing, L1/L2 cache invalidation, and lock contention on the shared task queue.

---

### Question 13: What causes a race condition on an unprotected counter?
**Answer:** `counter++` compiles into three discrete assembly instructions: LOAD memory into register, ADD 1 to register, and STORE register to memory. An interrupt or context switch between LOAD and STORE causes concurrent threads to read stale values, resulting in lost updates (empirically measured at 37.55% lost updates in Task 03).

---

### Question 14: How does a POSIX pipe signal End-of-File (EOF) to the reader?
**Answer:** In POSIX, `read(fd[0])` returns **0 (EOF)** if and only if **all write file descriptors referring to the pipe are closed**.

---

### Question 15: In Task 04, why does the child process hang if the parent omits `close(fd[1])`?
**Answer:** Because the parent still holds an open write descriptor (`fd[1]`), the kernel believes more data may arrive and suspends the child in the `read()` waiting state. Meanwhile, the parent calls `waitpid()` waiting for the child, resulting in a deadlock.

---

### Question 16: What is the difference between data copying in pipes versus shared memory?
**Answer:** 
- **Pipes:** Require **two memory copies** (User space $\rightarrow$ Kernel pipe buffer $\rightarrow$ User space).
- **Shared Memory:** Involves **zero kernel copies**. Both processes map the identical physical memory frames into their page tables, achieving over 5x higher throughput ($3,773\,\text{MB/s}$ vs $709\,\text{MB/s}$).

---

### Question 17: What are the three requirements for a solution to the critical-section problem?
**Answer:**
1. **Mutual Exclusion:** Only one thread inside the critical section at a time.
2. **Progress:** Selection of next thread cannot be postponed indefinitely.
3. **Bounded Waiting:** A bound exists on the number of times others enter before a waiting thread enters.

---

### Question 18: Why does Peterson's solution fail on modern superscalar processors?
**Answer:** Modern CPUs perform out-of-order execution and use speculative store buffers, which can reorder Stores followed by Loads (`Store-Load reordering`). Both threads can execute their store (`flag[i] = 1`) into store buffers and load `flag[j] = 0` from RAM simultaneously, causing both to enter the critical section together.

---

### Question 19: What was the result of the 100 consecutive runs verification in Task 05?
**Answer:** All 100 runs returned the exact sum of **210** ($20 \times 21 / 2$), verifying 100% deterministic thread safety and absence of race conditions.

---

### Question 20: What happened when the mutex was removed in Task 05?
**Answer:** Removing `pthread_mutex_t` caused concurrent producers and consumers to corrupt the shared circular buffer indices `in` and `out`, resulting in a corrupted sum of **184** instead of 210.

---

### Question 21: How did locking granularity affect Railway Booking throughput in Task 05?
**Answer:**
- **Global Lock:** Throughput collapsed from $21.05\,\text{M}$ to $1.41\,\text{M}$ ops/s at 16 threads (-93.3% loss).
- **Per-Coach Lock:** Throughput scaled up to **$62.50\,\text{Million}$ bookings/sec** ($44\times$ faster than global lock) by eliminating lock contention across coaches.

---

### Question 22: When should spinlocks be used instead of mutexes?
**Answer:** Spinlocks should only be used for **very short critical sections** (a few nanoseconds) in multiprocessor environments where context-switching overhead exceeds spinning time. For long critical sections (disk/DB I/O), mutexes are mandatory to avoid burning 100% CPU in busy-wait loops.

---

### Question 23: What are the four Coffman conditions for deadlock?
**Answer:** 1. Mutual Exclusion, 2. Hold and Wait, 3. No Preemption, 4. Circular Wait.

---

### Question 24: What is the cycle condition in a Resource Allocation Graph?
**Answer:**
- If every resource type has **exactly 1 instance**, a cycle in the RAG is both necessary and **sufficient** for deadlock.
- If resource types have **multiple instances**, a cycle indicates potential deadlock, but deadlock is not guaranteed.

---

### Question 25: What was the output of your Banker's Algorithm implementation?
**Answer:**
```text
initial state safe: P1 P3 P4 P0 P2
P1 request: granted, safe sequence P1 P3 P4 P0 P2
P4 request: must wait (not available)
P0 request: denied (unsafe)
```
100% exact match with the assignment PDF specification.

---

### Question 26: Why was P0's request for (0, 2, 0) denied in Task 06?
**Answer:** If granted, Available would drop to $(2, 1, 0)$. In that state:
- P0 needs $(7, 2, 3) > (2, 1, 0)$
- P1 needs $(0, 2, 0) > (2, 1, 0)$ (Resource 1: $2 > 1$)
- P2 needs $(6, 0, 0) > (2, 1, 0)$
- P3 needs $(0, 1, 1) > (2, 1, 0)$ (Resource 2: $1 > 0$)
- P4 needs $(4, 3, 1) > (2, 1, 0)$
No process can finish. The state is provably unsafe, so the request is denied.

---

### Question 27: What is the Big-O time complexity of Banker's Algorithm?
**Answer:** **$O(m \cdot n^2)$**, where $n$ is the number of processes and $m$ is the number of resource types.

---

### Question 28: What did your empirical benchmark show for 10, 100, and 1,000 processes?
**Answer:** Execution time scaled quadratically:
- $n = 10$: $1.42\,\mu\text{s}$
- $n = 100$: $118.50\,\mu\text{s}$ ($83.45\times$ growth)
- $n = 1,000$: $11,420.00\,\mu\text{s}$ ($11.42\,\text{ms}$)
This confirmed the $O(m \cdot n^2)$ theoretical prediction.

---

### Question 29: Why is Banker's Algorithm rarely used in real operating systems?
**Answer:**
1. It requires applications to declare their **maximum resource demand in advance**, which is impossible for dynamic research software.
2. The $O(m \cdot n^2)$ checking overhead on every single allocation causes severe performance degradation ($11.4\,\text{ms}$ delay per request at 1,000 tasks).

---

### Question 30: What deadlock strategy did you recommend for the research cluster?
**Answer:** A **Two-Tier Hybrid Strategy**:
1. **Prevention Tier:** Hierarchical ordering for static infrastructure (Storage before GPUs).
2. **Detection & Recovery Tier:** Dynamic $O(1)$ allocation for GPU memory and licenses with background cycle detection every 2 minutes and application-level checkpointing (`torch.save()`).
