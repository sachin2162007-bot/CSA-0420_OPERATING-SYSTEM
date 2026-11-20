# Fairness and Starvation Analysis in Disk Scheduling

## 1. The SSTF Starvation Problem
Shortest Seek Time First (SSTF) optimizes for local greedy efficiency:
$$\min_{r \in \mathcal{Q}} |\text{head} - r|$$
While this minimizes average seek time under light workloads, it exhibits catastrophic vulnerability under heavy or localized I/O:
- If new I/O requests continuously arrive within the spatial locality of the current head position (e.g., around cylinder $60$), the head will oscillate locally.
- Requests residing on outer tracks (e.g., cylinder $183$) or inner tracks (e.g., cylinder $14$) will experience **indefinite starvation**.

### Concrete Mathematical Proof
Consider an initial queue:
$$\mathcal{Q}_0 = \{65, 67, 183\}, \quad \text{head} = 53$$
- Head moves $53 \to 65 \to 67$.
- While at $67$, an application issues requests at cylinder $62, 60, 64$.
- SSTF services $67 \to 64 \to 62 \to 60$.
- Before reaching $183$, background database logging generates requests at $58, 59, 61$.
- Head remains trapped in the range $[55, 65]$.
- Request $183$ suffers unbounded latency: $\lim_{t \to \infty} W(183) = \infty$.

---

## 2. Linux Elevator Algorithms & Fairness Guarantees

### 2.1 Complete Fair Queueing (CFQ) & BFQ (Budget Fair Queueing)
- Assigns each process an I/O slice (or sector budget).
- When a process exhausts its budget, the scheduler switches to another queue, preventing runaway monopolization.

### 2.2 Deadline Scheduler (`mq-deadline`)
- Implements two separate FIFO queues alongside sorted sector trees:
  - **Read FIFO queue** (500 ms expiration timeout).
  - **Write FIFO queue** (5000 ms expiration timeout).
- If the request at the head of the read FIFO exceeds 500 ms, the scheduler abandons sorted elevator order and immediately dispatches the expired request.
- **Result**: Guarantees bounded latency while retaining $90\%+$ of elevator seek efficiency.

---

## 3. Rotational HDDs vs. Solid-State Drives (SSDs)

| Architectural Metric | Rotational HDD (Mechanical) | Solid-State Drive (NAND Flash / NVMe) |
| :--- | :--- | :--- |
| **Physical Moving Parts** | Actuator arm, read/write heads, spindle motor ($7,200\text{ RPM}$). | None (Solid-state semiconductor transistors). |
| **Seek Overhead** | Dominant delay ($4\text{ ms} - 12\text{ ms}$ seek + $4.16\text{ ms}$ rotational latency). | Zero mechanical seek latency ($\approx 10\text{ }\mu\text{s} - 50\text{ }\mu\text{s}$ access). |
| **Scheduling Goal** | Reorder requests by cylinder/LBA to minimize mechanical sweep. | Maximize IOPS parallelism across multiple flash channels and dies. |
| **Recommended Linux Scheduler** | `bfq` or `mq-deadline` (sorts by sector). | `none` (bypass) or `kyber` (minimizes queue CPU overhead). |
| **Parallel Submission Queues** | 1 hardware queue (NCQ depth = 32). | NVMe: Up to 64,000 queues with 64,000 commands each. |
