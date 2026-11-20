# Operating Systems Assignment 1: Operating System Types Judgment & Architectural Analysis
**Course Code:** CSA04  
**Scenario:** 500-Bed Smart Hospital Computing Infrastructure  
**Task:** Task 02 — Judge the Type of Operating System for Each Environment  

---

## 1. Taxonomy and Architectural Analysis of Operating System Types

Before selecting the optimal operating system architecture for each of the six hospital environments, we establish the technical characteristics, scheduling paradigms, and resource management mechanisms of the seven primary operating system categories.

### 1.1 Mainframe Operating Systems
* **Architectural Profile:** Designed for room-sized enterprise machines (e.g., IBM z/OS, z/VM, Linux on IBM Z) featuring massive I/O channel subsystems, cryptographic hardware co-processors, and Logical Partitioning (LPAR) hypervisors.
* **Workload Paradigms:** Supports three concurrent execution models:
  1. **Batch Processing:** Non-interactive, scheduled bulk jobs (e.g., nightly insurance claims billing, epidemiological aggregation).
  2. **Transaction Processing (OLTP):** High-volume, short-duration atomic state updates (e.g., CICS/IMS transaction monitors).
  3. **Timesharing:** Concurrent interactive terminal sessions for hundreds or thousands of remote users.
* **Strengths & Limitations:** Exceptional Reliability, Availability, and Serviceability (RAS) with hardware-level instruction retry and redundant I/O channels, but entails extreme capital expenditure (CapEx), proprietary licensing, and specialized administrative overhead.

### 1.2 Desktop Operating Systems (Personal Computer OS)
* **Architectural Profile:** Preemptive multitasking General-Purpose Operating Systems (e.g., Workstation Linux, Windows 11 Enterprise, macOS) optimized for single-user interactive responsiveness and rich peripheral ecosystems.
* **Key Mechanisms:** Compositing window managers, GPU-accelerated display servers (Wayland/X11, WDDM), virtual memory paging with swap, and fairness-oriented CPU schedulers (such as Linux EEVDF/CFS) that boost interactive I/O-bound foreground threads.
* **Strengths & Limitations:** High ergonomic productivity and broad third-party clinical software support, but non-deterministic scheduling latencies, large memory footprints, and expanded attack surfaces make them unsuitable for embedded life-support or unattended high-availability clustering.

### 1.3 Multiprocessor Operating Systems
* **Architectural Profile:** Operating systems managing two or more tightly coupled CPUs/cores sharing physical memory, system bus/interconnect (e.g., UPI, Infinity Fabric), I/O controllers, and a single system clock.
* **Sub-Architectures:**
  * **Asymmetric Multiprocessing (AMP):** A designated Master Processor executes the OS kernel, handles all interrupts, and schedules user tasks onto Slave Processors (or assigns fixed functional roles to specific cores).
  * **Symmetric Multiprocessing (SMP):** Every processor core runs an identical copy of the OS kernel, shares the global/per-core run queues, services interrupts, and synchronizes access to kernel data structures via fine-grained spinlocks, RCU (Read-Copy-Update), and atomic compare-and-swap (`CAS`) primitives. Includes Non-Uniform Memory Access (NUMA) topologies where memory access time depends on the physical socket locality.

### 1.4 Distributed Operating Systems
* **Architectural Profile:** A collection of physically separate, loosely coupled autonomous compute nodes connected via a network that appears to users and applications as a single coherent computing system (Single System Image — SSI, e.g., Amoeba, Plan 9, Sprite).
* **Key Mechanisms:** Transparent global process migration, distributed shared memory (DSM), unified global namespace, and location-independent inter-process communication (IPC).
* **Distinction from Network OS:** Unlike a Network Operating System where users explicitly log into specific remote hosts (`ssh`, explicit file transfer), a true Distributed OS hides node boundaries completely inside the kernel.

### 1.5 Clustered Operating Systems
* **Architectural Profile:** Two or more individual systems (nodes) coupled via high-speed, low-latency local interconnects (e.g., InfiniBand, RoCE, redundant 25/100 GbE) and shared storage (SAN/NVMe-oF) or replicated state, managed by cluster middleware and Distributed Lock Managers (DLM).
* **Operational Modes:**
  * **High-Availability (HA) Active-Passive / Active-Active:** Continuous heartbeat monitoring (`Corosync`/`Pacemaker`) and quorum consensus (Raft/Paxos) with STONITH (*Shoot The Other Node In The Head*) fencing to prevent split-brain data corruption during automatic failover.
  * **High-Performance Computing (HPC) Beowulf Clustering:** Parallel job execution across nodes via MPI (Message Passing Interface).

### 1.6 Real-Time Operating Systems (Hard vs. Soft RTOS)
* **Hard Real-Time Operating Systems (Hard RTOS):**
  * **Definition:** Deterministic operating systems (e.g., QNX Neutrino, VxWorks, FreeRTOS, Zephyr) where missing a single task deadline ($t_{\text{completion}} > D_i$) constitutes a catastrophic system failure.
  * **Mechanisms:** Fixed-priority preemptive scheduling (Rate Monotonic Analysis — RMA) or Earliest Deadline First (EDF), priority inheritance mutexes to bound priority inversion, static/pinned physical memory (no demand paging or swap), and bounded $\mathcal{O}(1)$ interrupt service latency.
* **Soft Real-Time Operating Systems (Soft RTOS):**
  * **Definition:** Systems (e.g., Linux with `PREEMPT_RT` or `SCHED_DEADLINE`, multimedia kernels) where meeting deadlines is highly desirable for quality of service (QoS), and real-time tasks retain permanent priority over normal tasks, but occasional deadline misses cause degraded performance (e.g., dropped video frames or delayed UI refresh) rather than catastrophic failure.

### 1.7 Handheld Operating Systems (Mobile OS)
* **Architectural Profile:** Power-constrained, event-driven operating systems (e.g., Android Enterprise, iOS/iPadOS) engineered for battery-powered mobile System-on-Chip (SoC) architectures (ARM big.LITTLE).
* **Key Mechanisms:** Aggressive dynamic voltage and frequency scaling (DVFS), deep C-state/suspend sleep states with opportunistic wakeup coalescing, low-memory killer (LMK) daemons instead of traditional disk swap, mandatory per-application UID sandboxing, and zero-copy capacitive touch/display pipelines.

---

## 2. Operating System Judgment for the Six Hospital Environments

### 2.1 Bedside Patient Monitors
* **Selected OS Type:** **Hard Real-Time Operating System (Hard RTOS)** (embedded microkernel/RTOS).
* **Technical Justification:** Bedside monitors continuously sample physiological waveforms (ECG, $\text{SpO}_2$, invasive blood pressure) and execute arrhythmia detection algorithms across 500 beds. Detecting ventricular fibrillation or apnea requires strictly bounded worst-case execution time ($\text{WCET} \le 2\text{ ms}$). Only a Hard RTOS guarantees zero paging delays, deterministic interrupt dispatch, and priority-inheritance locking so life-critical alarms never stall behind background network or display tasks.
* **Justification for Rejected Alternatives:**
  * *Soft RTOS / Desktop OS:* Unbounded scheduling jitter from virtual memory page faults, kernel spinlocks, and filesystem writeback buffers can delay critical alarms beyond safe physiological windows.
  * *Handheld OS:* Aggressive deep-sleep state transitions and background task killers (`LMK`) risk suspending continuous waveform monitoring daemons.

### 2.2 Nurses' Handheld Tablets
* **Selected OS Type:** **Handheld Operating System (Mobile OS)** with Soft Real-Time UI/Notification Extensions.
* **Technical Justification:** Nurses roam across hospital wards during 12-hour shifts carrying battery-powered tablets for barcode medication administration (BCMA), vitals charting, and clinical alert reception. A Handheld OS optimizes heterogeneous ARM cores for full-shift battery endurance, provides sub-$16.6\text{ ms}$ touch response, enforces hardware-backed full-disk encryption (FDE), and isolates clinical apps inside strict sandboxes.
* **Justification for Rejected Alternatives:**
  * *Desktop OS:* Lacks aggressive power-state governance (`wakelock` suspension) and mobile radio handoff optimization, exhausting tablet batteries within 2–3 hours.
  * *Hard RTOS:* Lacks rich multimedia GUI frameworks, web rendering engines, and dynamic application lifecycle management required for modern EHR mobile clients.

### 2.3 Doctors' Desktop Workstations
* **Selected OS Type:** **Desktop Operating System** on **Symmetric Multiprocessor (SMP)** Hardware.
* **Technical Justification:** Physicians concurrently execute complex multi-monitor workflows: EHR charting, multi-planar PACS radiology viewers, clinical decision support browsers, and voice dictation. A modern Desktop OS running on a multi-core SMP workstation leverages preemptive time-sharing, virtual memory protection, and GPU window compositing to deliver responsive interactive multitasking alongside enterprise smartcard SSO and Role-Based Access Control (RBAC).
* **Justification for Rejected Alternatives:**
  * *Handheld OS:* Restrictive windowing models and mobile SoC thermal envelopes cannot drive dual high-luminance 4K diagnostic radiology monitors or heavy local 3D DICOM rendering.
  * *Mainframe OS:* Designed for centralized batch/transaction processing; lacks local workstation GPU display servers and interactive peripheral ergonomics.

### 2.4 Central Patient Database Server
* **Selected OS Type:** **Clustered Operating System** (High-Availability Active-Active / Active-Passive Server Cluster with Multiprocessor SMP Nodes; or Enterprise Mainframe OS Cluster).
* **Technical Justification:** Serving as the authoritative store for 500 bedside telemetry streams, medication orders, allergy profiles, and clinical notes, the central database cannot tolerate a single point of failure (SPOF). A Clustered OS combining multi-socket SMP nodes with redundant heartbeat interconnects, quorum fencing, and synchronous replicated storage guarantees continuous availability ($99.999\%$ uptime) and horizontal transaction scaling.
* **Justification for Rejected Alternatives:**
  * *Standalone Desktop OS:* Lacks enterprise I/O multipathing, Distributed Lock Managers (DLM), ECC memory error containment, and automated cluster failover.
  * *True Distributed OS (Single System Image):* Kernel-level global distributed shared memory (DSM) across nodes introduces severe network page-thrashing latency under high-contention ACID database locks compared to explicit shared-disk or shared-nothing database clustering over a Clustered OS.

### 2.5 MRI Imaging Server
* **Selected OS Type:** **Symmetric Multiprocessor (SMP) Operating System** with **Soft Real-Time** Acquisition Threads & GPU Compute Scheduling.
* **Technical Justification:** MRI reconstruction transforms gigabytes of raw $k$-space radiofrequency data into 3D volumetric DICOM slices via multidimensional Fast Fourier Transforms (FFTs) and iterative reconstruction. An SMP OS with NUMA-aware memory allocation, huge-page support, and high-throughput DMA/GPU scheduling maximizes parallel compute throughput across all CPU cores and co-processors while meeting soft real-time deadlines (reconstructing volumes within seconds while the patient remains inside the bore).
* **Justification for Rejected Alternatives:**
  * *Hard RTOS:* Optimized for minimal latency on small deterministic payloads rather than maximizing bulk memory bandwidth, vector SIMD utilization, and multi-gigabyte GPU DMA transfers.
  * *Asymmetric Multiprocessing (AMP) OS:* Creates a severe kernel/scheduling bottleneck on the single Master CPU when coordinating tens of thousands of parallel FFT matrix blocks (detailed in Section 3).

### 2.6 Partner-Hospital Network Connection
* **Selected OS Type:** **Hardened Network Gateway / Clustered Bastion Operating System** (with Soft Real-Time Packet Processing & Distributed Protocol Interoperability).
* **Technical Justification:** The perimeter gateway bridges the internal hospital intranet with external partner hospitals for Health Information Exchange (HL7 FHIR), teleradiology, and trauma transfers. Rather than exposing internal kernels via a tightly coupled Distributed OS, this boundary requires a hardened, minimal-footprint Network/Appliance OS deployed as a high-availability active-passive cluster pair running stateful packet filtering (`nftables`/eBPF), IPsec/WireGuard VPN termination, mutual TLS (mTLS), and Intrusion Prevention Systems (IPS).
* **Justification for Rejected Alternatives:**
  * *Tightly Coupled Distributed OS (Single System Image):* Extending a single global kernel namespace across an untrusted inter-hospital WAN violates security isolation; a compromise at a partner hospital would immediately grant kernel-level access to the local hospital.
  * *Desktop OS:* Contains unnecessary GUI subsystems, user-space services, and unhardened network defaults that expand the perimeter attack surface.

---

## 3. Deep-Dive Architectural Analysis I: Symmetric (SMP) vs. Asymmetric Multiprocessing (AMP) for the MRI Imaging Server

### 3.1 Workload Characteristics of MRI Reconstruction
An MRI scan acquires complex-valued spatial frequency samples in $k$-space across multiple phased-array receiver coils (e.g., $C = 32 \text{ to } 64$ coils) for a 3D volume of $D \times D \times D$ voxels (e.g., $512 \times 512 \times 256$). Reconstruction requires:
1. Coil sensitivity calibration and parallel imaging unfolding (SENSE / GRAPPA).
2. 3D Inverse Fast Fourier Transform (IFFT) across all coils.
3. Root-sum-of-squares coil combination, non-local means denoising, and DICOM slice serialization.

This workload is **embarrassingly data-parallel** across slices and receiver coils, generating thousands of independent compute threads and massive DMA I/O interrupts.

### 3.2 Asymmetric Multiprocessing (AMP) Architecture on MRI Server
In an AMP architecture with $P$ total processor cores:
* **Master Core ($1$ Core):** Exclusively executes the OS kernel, handles all hardware DMA interrupts from the MRI spectrometer and NVMe storage, manages memory allocation, and dispatches tasks to the $P - 1$ slave cores.
* **Slave Cores ($P - 1$ Cores):** Execute user-space FFT compute threads only; every system call (`mmap`, `read`, `write`, `ioctl` to GPU) or page fault traps back to the single Master Core.
* **Bottleneck Analysis:**
  * As the number of cores $P$ scales (e.g., $P = 64$ or $128$ cores in a modern imaging server), the single Master Core saturates at $100\%$ utilization servicing DMA interrupts, page-table updates, and system call traps.
  * Slave cores sit idle waiting in dispatch queues (**Master-Core Serialization Bottleneck**).
  * Furthermore, if the Master Core fails, the entire MRI reconstruction server crashes immediately.

### 3.3 Symmetric Multiprocessing (SMP) Architecture on MRI Server
In an SMP (and NUMA-aware SMP) architecture with $P$ peer processor cores:
* **Peer Execution:** All $P$ cores run the kernel symmetrically. Any core can service NVMe/network/GPU interrupts (via Interrupt Affinity / RSS), allocate memory from its local NUMA node, and pull FFT worker threads from per-core run queues with work-stealing.
* **Synchronization & Scaling:** Using lock-free ring buffers, Read-Copy-Update (RCU), and per-CPU slab allocators, kernel contention is minimized. All $P$ cores execute parallel 3D FFT slices simultaneously.
* **Quantitative Throughput & Speedup Comparison (Amdahl's Law):**
  Let $f \in (0,1)$ be the parallelizable fraction of the MRI reconstruction workload (typically $f \approx 0.95$ for 3D multi-coil FFTs) and $(1-f)$ be the sequential/OS-kernel overhead fraction.
  * In **SMP**, parallel speedup across $P$ cores follows Amdahl's bound with distributed kernel overhead $\epsilon_{\text{lock}}$:
    $$S_{\text{SMP}}(P) = \frac{1}{(1 - f) + \frac{f}{P} + \epsilon_{\text{lock}}}$$
  * In **AMP**, all kernel calls and I/O interrupts serialize on the single Master Core. If each slave task requires a kernel/master service fraction $\alpha$, throughput saturates strictly at $P^* = \frac{1}{\alpha}$, leaving $P - P^*$ cores starved:
    $$S_{\text{AMP}}(P) = \min\left(P, \frac{1}{\alpha}\right)$$
* **Conclusion:** **SMP is overwhelmingly superior for the MRI Imaging Server**, delivering near-linear throughput scaling across multi-core CPUs and GPU DMA channels, whereas AMP creates a single-core kernel bottleneck and single point of failure. *(Note: A minor hybrid nuance exists only inside the MRI scanner's internal hardware controller, where a dedicated microcontroller/RTOS core handles hard real-time RF/gradient pulses while an SMP server handles image reconstruction).*

---

## 4. Deep-Dive Architectural Analysis II: Clustered OS with Failover vs. Single Large Multiprocessor Server for the Central Patient Database

### 4.1 Competing Architectural Proposals
* **Architecture A (Recommended): Clustered Operating System with Automated Failover.** A cluster of $K$ independent multi-core server nodes (e.g., $K = 3$ or $4$ nodes) running a Clustered OS connected via redundant high-speed interconnects, shared/replicated NVMe storage, quorum consensus, and automatic failover.
* **Architecture B (Alternative Concept): Single Large Monolithic Multiprocessor Server (Scale-Up SMP/NUMA).** A single massive shared-memory multiprocessor server (e.g., an 8-socket, 256-core monolithic machine with terabytes of RAM) running a single OS kernel instance without clustering.

### 4.2 Rigorous Evaluation Across Six Engineering Dimensions

#### 1. Availability & Mathematical Reliability Modeling
Let $A_{\text{node}} \in (0,1)$ represent the steady-state availability of a single server node (defined by Mean Time Between Failures and Mean Time To Repair):
$$A_{\text{node}} = \frac{\text{MTBF}}{\text{MTBF} + \text{MTTR}}$$
* **Single Large Multiprocessor Server (Architecture B):** System availability equals the availability of the single OS kernel and chassis:
  $$A_{\text{single}} = A_{\text{node}}$$
  Furthermore, any kernel patch, OS upgrade, or motherboard maintenance requires a full reboot, causing **100% scheduled downtime** across the 500-bed hospital.
* **Clustered OS with $K$ Nodes (Architecture A):** Assuming independent node failures and a cluster capable of surviving up to $K-1$ node failures (or quorum majority $\lfloor K/2 \rfloor$), theoretical availability approaches:
  $$A_{\text{cluster}} = 1 - (1 - A_{\text{node}})^K \cdot (1 + p_{\text{failover\_fault}})$$
  Even accounting for imperfect failover probability $p_{\text{failover\_fault}}$, a Clustered OS supports **zero-downtime rolling upgrades** (draining and rebooting one node at a time while remaining nodes serve all 500 beds).

#### 2. Single Point of Failure (SPOF)
* **Single Large Multiprocessor:** Although a high-end scale-up server has redundant power supplies and ECC RAM, **the single OS kernel instance, system backplane/midplane, and physical chassis remain critical Single Points of Failure**. A single kernel panic (e.g., from a buggy storage driver), uncorrectable multi-bit memory error in kernel space, or localized rack fire brings down the entire hospital database.
* **Clustered OS:** Eliminates hardware, kernel, and rack-level SPOFs. Each node runs an isolated kernel instance in separate failure domains (and potentially separate fire zones/rooms).

#### 3. Failover Mechanics & Recovery Time Objective (RTO)
* **Single Large Multiprocessor:** **No automated failover exists.** If the system halts, recovery requires hardware diagnostics, BIOS POST (which takes 10–20 minutes on multi-terabyte RAM systems), OS boot, and database crash recovery (`WAL` replay), yielding $\text{RTO} \approx 15 \text{ to } 60\text{ minutes}$—unacceptable during emergency surgeries or ICU admissions.
* **Clustered OS:** Heartbeat daemons detect node failure within $1\text{--}3\text{ seconds}$, execute STONITH fencing to isolate the failed node's storage paths, promote a standby/active peer, and reassign the Virtual IP (VIP). Total failover $\text{RTO} \approx 2 \text{ to } 10\text{ seconds}$ with zero committed transaction loss ($\text{RPO} = 0$).

#### 4. Cost (CapEx and OpEx)
* **Single Large Multiprocessor:** Exhibits **super-linear hardware cost scaling** ($\text{Cost} \propto C^{\gamma}$ where $\gamma > 1.5$ for core count $C$). Specialized 8-socket glueless/node-controller NUMA motherboards, proprietary interconnects, and enterprise per-core licensing make a single massive machine extraordinarily expensive, with zero fault-tolerance protection if that single chassis fails.
* **Clustered OS:** Built from **commodity-off-the-shelf (COTS) 2-socket SMP servers**, yielding linear, cost-effective horizontal scaling. While cluster software licensing and SAN/replication networking add moderate software/network costs, total cost-per-unit-of-availability is substantially lower.

#### 5. Throughput (Read vs. Write Scaling & Coherence Overhead)
* **Single Large Multiprocessor:**
  * *Advantage:* All threads share a single physical address space. Inter-process communication and lock acquisition occur via hardware cache coherence (MESIF/MOESI protocols) over internal CPU interconnects in nanoseconds ($\sim 50\text{--}150\text{ ns}$), avoiding network serialization overhead for write-heavy cross-table transactions.
  * *Limitation:* Beyond 4–8 sockets, cross-socket NUMA cache-coherence invalidation traffic saturates the hardware interconnect.
* **Clustered OS:**
  * *Advantage:* Scales aggregate **read throughput** and ingestion concurrency almost linearly across $K$ nodes.
  * *Limitation:* Distributed write transactions require network-based Distributed Lock Management (DLM) and Two-Phase Commit (2PC) or Raft log replication across the cluster interconnect ($\sim 50\text{--}500\,\mu\text{s}$), imposing a coordination tax on write-contention workloads.

#### 6. Memory Complexity & Cache Coherence State
* **Single Large Multiprocessor:**
  * Maintains a single unified kernel page table and a single database buffer pool in shared RAM:
    $$\text{Space Complexity (OS & Buffer Pool)} = \mathcal{O}(M_{\text{data}} + P)$$
    where $M_{\text{data}}$ is the working set size and $P$ is total threads. However, hardware NUMA directory coherence tables grow as $\mathcal{O}(S \cdot B_{\text{cache}})$ across $S$ sockets and $B_{\text{cache}}$ cache lines.
* **Clustered OS:**
  * Each of the $K$ nodes runs its own OS kernel, local buffer cache, and Distributed Lock Manager (DLM) resource directory for $L$ active distributed locks:
    $$\text{Space Complexity (Cluster-Wide RAM)} = \mathcal{O}(K \cdot M_{\text{kernel}} + M_{\text{data\_replicated}} + K \cdot L)$$
  * While clustering duplicates kernel footprints and cached pages across $K$ nodes, modern DRAM is inexpensive compared to the patient-safety requirement of continuous availability.

---

## 5. Comprehensive Time and Space Complexity Analysis Across OS Types

The table and derivations below formalize the asymptotic time and space complexities of the core OS mechanisms governing each hospital environment ($N = 500$ beds, $P$ runnable tasks/threads, $C$ processor cores, $K$ cluster nodes, $R$ database records, $V = D^3$ MRI voxels):

### 5.1 Scheduling, Synchronization, and Subsystem Complexities

1. **Hard RTOS (Bedside Monitors) — Bitmap Fixed-Priority Preemptive Scheduler:**
   * With $Q$ fixed priority levels (typically $Q = 32$ or $256$) mapped to a bitmask and evaluated via single-cycle hardware Count Leading Zeros (`CLZ`):
     $$\text{Time Complexity (Enqueue / Dequeue / Next-Task Dispatch)} = \mathcal{O}(1)$$
     $$\text{Space Complexity (Static Task Control Blocks for } P \text{ fixed tasks)} = \mathcal{O}(P) = \mathcal{O}(1)$$

2. **Handheld & Desktop OS (Nurses' Tablets & Doctors' Workstations) — Fair Preemptive Scheduler (e.g., Red-Black Tree / EEVDF):**
   * Managing $P$ dynamic interactive and background threads ordered by virtual runtime (`vruntime`) or virtual deadline:
     $$\text{Time Complexity (Task Insertion / Tree Rebalance)} = \mathcal{O}(\log P)$$
     $$\text{Time Complexity (Pick Next Task via Cached Leftmost Pointer)} = \mathcal{O}(1)$$
     $$\text{Space Complexity (Runqueue & Per-Process Page Tables)} = \mathcal{O}(P + V_{\text{pages}})$$

3. **Multiprocessor OS (MRI Imaging Server) — SMP Work-Stealing vs. AMP Master Dispatch:**
   * Let $P$ tasks execute across $C$ processor cores, and let $V = D^3$ be the 3D MRI voxel matrix size:
     * **AMP Dispatch Time Complexity:** All $P$ scheduling events and system calls serialize through $1$ Master Core:
       $$T_{\text{dispatch, AMP}} = \mathcal{O}(P) \quad \text{(Sequential bottleneck)}$$
     * **SMP Distributed Runqueue Dispatch Time Complexity:** Parallel dispatch across $C$ independent per-core queues:
       $$T_{\text{dispatch, SMP}} = \mathcal{O}\left(\frac{P}{C}\right) \text{ average}, \quad \mathcal{O}(\log(P/C)) \text{ per-core tree lookup}$$
     * **Parallel 3D FFT Reconstruction Time & Space Complexity on $C$ Cores:**
       $$T_{\text{FFT, SMP}} = \mathcal{O}\left(\frac{D^3 \log D}{C}\right) = \mathcal{O}\left(\frac{V \log V}{C}\right), \quad S_{\text{FFT, SMP}} = \mathcal{O}(D^3 + C)$$

4. **Clustered OS (Central Patient Database Server) — Heartbeat, Consensus, and Distributed Locking:**
   * Across $K$ cluster nodes managing $L$ distributed locks and $R$ patient records:
     * **All-to-All Heartbeat Failure Detection Message Complexity per Interval:**
       $$T_{\text{heartbeat}} = \mathcal{O}(K^2) \text{ messages} \quad (\text{or } \mathcal{O}(K) \text{ with multicast/ring topology})$$
     * **Quorum Consensus / Distributed Commit (Raft / 2PC) per Write Transaction:**
       $$T_{\text{commit}} = \mathcal{O}(K) \text{ network messages}, \quad \mathcal{O}(1) \text{ round-trip latency rounds}$$
     * **Database Indexed Lookup + Distributed Lock Lookup:**
       $$T_{\text{query}} = \mathcal{O}(\log_b R) + \mathcal{O}(1)_{\text{DLM hash}}, \quad S_{\text{cluster}} = \mathcal{O}(K \cdot L + R)$$