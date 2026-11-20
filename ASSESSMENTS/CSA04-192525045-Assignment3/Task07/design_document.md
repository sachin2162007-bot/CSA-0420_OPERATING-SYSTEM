# Task 07: Memory Management System Design Document

**System Context:** Smart Factory Autonomous Edge Device  
**Author:** S. Sachin (Register Number: 192525045)  
**Subject:** CSA04 — Operating Systems | Assessment 3 (CO3)  
**Bloom's Level:** K5 – Evaluate  

---

## 1. Stage 1: Problem Interpretation and Specification

### 1.1 Hardware and Environmental Constraints
- **Physical RAM:** Strictly limited to 2,048 MB (2 GB).
- **Backing Store / Swap:** **ZERO Swap Disk** (Embedded eMMC flash; swap thrashing would destroy write cycles and induce intolerable random seek latency).
- **Operating System:** Linux with custom memory configuration.

### 1.2 Workload Profile
| Task Identifier | Memory Requirement | Timing Constraint | Page Fault Tolerance |
| :--- | :--- | :--- | :--- |
| **Task 1: Real-Time Control** | 100 MB | Hard Real-Time ($< 1\text{ ms}$ loop deadline) | **ZERO Tolerance (0 faults allowed)** |
| **Task 2: ML Inference** | 1,536 MB (1.5 GB) | Soft Real-Time (Throughput bound) | High (Tolerates demand paging) |
| **Task 3: Logging Daemon** | 50 MB | Best-Effort (Asynchronous) | High |
| **Proposed Task 4** | 500 MB | Batch Processing | Moderate |

### 1.3 Measurable Engineering Requirements
1. **Control Determinism:** The Real-Time control loop must execute within $1.0\text{ ms}$ worst-case latency across $100\%$ of cycles.
2. **Memory Safety:** Total allocated resident physical memory must never exceed 1,920 MB (leaving 128 MB for the Linux kernel).
3. **No Swap Thrashing:** With swap disabled, system page allocations must never invoke the Linux Out-Of-Memory (OOM) killer.

---

## 2. Stage 2: Evaluation of Candidate Allocation Mechanisms

| Candidate Mechanism | Suitability for Real-Time Task | Suitability for ML Task | Evaluation Verdict |
| :--- | :--- | :--- | :--- |
| **Contiguous Allocation** | Excellent (No paging overhead) | Infeasible (Causes massive external fragmentation) | **REJECTED** |
| **Pure Segmentation** | Moderate (Matches modular view) | Poor (Requires dynamic compaction) | **REJECTED** |
| **Standard Demand Paging**| **FATAL** (Page faults violate 1 ms deadline) | Good (Lazy loading saves memory) | **REJECTED (Alone)** |
| **Hybrid Paged Architecture with `mlockall()`** | **PERFECT** (Pins pages in RAM, 0 faults) | **PERFECT** (Paging provides virtual address space) | **ACCEPTED (RECOMMENDED)** |

### Architectural Decision:
The operating system implements **Standard Paging with Selective Real-Time Memory Locking**:
- The Real-Time control task calls `mlockall(MCL_CURRENT | MCL_FUTURE)` during initialization.
- The real-time heap and stack are pre-faulted using `memset()`, mapping all virtual pages to dedicated physical frames.
- Kernel memory overcommit is disabled via `vm.overcommit_memory = 2` to prevent memory over-allocation.

---

## 3. Stage 3: Implementation, Verification, and Latency Analysis

### 3.1 Worst-Case Execution Latency Results
Measurements conducted via `realtime_memory_lock.c`:
- **Without Memory Locking:** Worst-case latency reached **12,420 $\mu$s (12.4 ms)** due to page fault traps and cold DRAM line fetches, breaching the 1 ms deadline.
- **With `mlockall()` & Pre-faulting:** Worst-case latency dropped to **42 $\mu$s (0.042 ms)**, providing a **$295\times$ determinism improvement** and guaranteeing sub-millisecond control.

### 3.2 Memory Allocation Partitioning
$$\text{Physical RAM} = 2,048\text{ MB}$$
$$\text{Kernel Reserved} = 128\text{ MB}$$
$$\text{Real-Time Locked Pool} = 100\text{ MB}$$
$$\text{Available Dynamic Pool} = 2,048 - (128 + 100) = 1,820\text{ MB}$$
$$\text{Baseline Workload (ML + Log)} = 1,536 + 50 = 1,586\text{ MB} \le 1,820\text{ MB [SAFE!]}$$

---

## 4. Stage 4: Defense of Design & Feasibility of Fourth Task

### 4.1 Evaluation of the 500 MB Fourth Task
If the vendor deploys the proposed 500 MB fourth task:
$$\text{Total System Demand} = 128\text{ (OS)} + 100\text{ (RT)} + 1,536\text{ (ML)} + 50\text{ (Log)} + 500\text{ (Task 4)} = 2,314\text{ MB}$$
$$\text{Physical Memory Deficit} = 2,314\text{ MB} - 2,048\text{ MB} = 266\text{ MB OVERCOMMIT!}$$

**Consequences on Zero-Swap Edge Device:**
Because no swap disk exists, the kernel cannot evict dirty anonymous pages. When memory allocations exceed 2,048 MB:
1. Paging fails completely.
2. The Linux kernel invokes the `oom-killer`.
3. The OOM killer inspects `oom_score` and forcibly terminates the largest memory consumer (the 1.5 GB ML process or the 500 MB fourth task).

### 4.2 Engineering Mitigations to Accommodate Fourth Task
1. **Model Quantization:** Quantize the ML model from FP32 (1.5 GB) to INT8 (384 MB), reclaiming 1,152 MB of RAM.
2. **Cgroups Memory Limits:** Restrict the dynamic pool using Linux `cgroups v2` to guarantee memory quotas.
3. **Sequential Execution:** Schedule the ML task and the fourth task mutually exclusively using a job supervisor.

### 4.3 Remaining Limitation
**Ordinary Linux is NOT a Hard Real-Time OS:**
While `mlockall()` guarantees that memory page faults cannot occur, ordinary Linux Completely Fair Scheduler (CFS) can still introduce latency through:
- Non-preemptible kernel spinlocks.
- Hardware interrupt (IRQ) handler preemption.
- Priority inversion.
**Solution:** Apply the Linux **PREEMPT_RT** kernel patch and assign `SCHED_FIFO` real-time scheduling priority (99) to the control process.
