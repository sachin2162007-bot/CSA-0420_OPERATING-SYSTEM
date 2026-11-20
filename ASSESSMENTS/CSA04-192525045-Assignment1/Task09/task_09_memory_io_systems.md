# Task 09 — Memory Hierarchy and I/O Systems Evaluation

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assignment 1 (CO1)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Scenario:** 500-Bed Smart Hospital Computing Infrastructure  
**Bloom's Level:** K5 – Evaluate  

---

## 1. Executive Summary

A modern 500-bed hospital processes massive volumes of heterogeneous medical data: real-time patient telemetry ($1	ext{--}5\,	ext{KB/s}$ per patient), electronic health records ($50\,	ext{KB}$ transactional records), and high-resolution magnetic resonance imaging (MRI) scans ($500\,	ext{MB}$ per scan, up to several gigabytes per patient examination).

Because memory technology presents an inherent tradeoff between access speed, capacity, and cost per gigabyte, no single storage medium satisfies all clinical needs. This evaluation analyzes the levels of the memory hierarchy, formulates an optimal data-placement plan for hospital workloads, evaluates I/O methodologies for MRI acquisition, and models the performance impact of Direct Memory Access (DMA) versus interrupt-driven I/O.

---

## 2. The Memory Hierarchy: Latency, Bandwidth, and Capacity

```
                  +-----------------------------------+
                  |      REGISTERS (<1 ns, ~1 KB)     |
                  +-----------------------------------+
                                    |
                  +-----------------------------------+
                  |      L1 CACHE (1-2 ns, ~64 KB)    |
                  +-----------------------------------+
                                    |
                  +-----------------------------------+
                  |     L2 CACHE (3-10 ns, ~512 KB)   |
                  +-----------------------------------+
                                    |
                  +-----------------------------------+
                  |     L3 CACHE (10-25 ns, ~32 MB)   |
                  +-----------------------------------+
                                    |
                  +-----------------------------------+
                  |   MAIN MEMORY RAM (50-100 ns, GB) |
                  +-----------------------------------+
                                    |
                  +-----------------------------------+
                  |    NVMe SSD FLASH (10-50 us, TB)  |
                  +-----------------------------------+
                                    |
                  +-----------------------------------+
                  |   ENTERPRISE HDD (5-10 ms, TB-PB) |
                  +-----------------------------------+
                                    |
                  +-----------------------------------+
                  | CLOUD ARCHIVE STORAGE (50-200 ms) |
                  +-----------------------------------+
```

### Comprehensive Storage Tier Metrics

| Hierarchy Tier | Typical Access Latency | Bandwidth | Typical Capacity | Cost per GB | Volatility |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **CPU Registers** | $0.5 - 1.0\,	ext{ns}$ | $>1\,	ext{TB/s}$ | $1 - 2\,	ext{KB}$ | Extremely High | Volatile |
| **L1 CPU Cache** | $1.0 - 2.0\,	ext{ns}$ | $>500\,	ext{GB/s}$ | $64 - 128\,	ext{KB}$ | Integrated | Volatile |
| **L2 CPU Cache** | $3.0 - 8.0\,	ext{ns}$ | $>250\,	ext{GB/s}$ | $512\,	ext{KB} - 1\,	ext{MB}$| Integrated | Volatile |
| **L3 Shared Cache**| $10 - 25\,	ext{ns}$ | $>100\,	ext{GB/s}$ | $16 - 64\,	ext{MB}$ | Integrated | Volatile |
| **Main Memory (DRAM)**| $60 - 90\,	ext{ns}$ | $30 - 60\,	ext{GB/s}$ | $16 - 256\,	ext{GB}$ | $pprox \$3 - \$5$ | Volatile |
| **NVMe PCIe SSD** | $10 - 30\,\mu	ext{s}$ | $3 - 7\,	ext{GB/s}$ | $1 - 10\,	ext{TB}$ | $pprox \$0.10$ | Non-Volatile |
| **SATA SSD** | $50 - 100\,\mu	ext{s}$ | $500\,	ext{MB/s}$ | $1 - 4\,	ext{TB}$ | $pprox \$0.06$ | Non-Volatile |
| **Enterprise HDD**| $5 - 10\,	ext{ms}$ | $150 - 250\,	ext{MB/s}$| $10 - 20\,	ext{TB}$ | $pprox \$0.015$| Non-Volatile |
| **Cloud Object Store**| $50 - 200\,	ext{ms}$ | $100\,	ext{MB/s} - 1\,	ext{GB/s}$| Petabytes | $pprox \$0.005$| Non-Volatile |

---

## 3. Justified Data-Placement Strategy for Hospital Workloads

### 3.1 Bedside Patient Monitor Readings
* **Placement:** **CPU Registers $	o$ L1/L2 Cache $	o$ Pinned Physical DRAM Ring Buffer $	o$ NVRAM.**
* **Justification:** Sensor telemetry requires continuous DSP filtering (QRS complex detection, pulse oximetry calculation) at $1\,	ext{kHz}$. Reading and processing must complete in nanoseconds without memory bus contention or disk I/O. Data is retained in a 24-hour circular DRAM buffer; critical alarm events trigger asynchronous flushing to non-volatile flash.

### 3.2 Patient Electronic Health Records (EHR)
* **Placement:** **In-Memory Buffer Pool (DRAM) $	o$ NVMe SSD RAID-10 $	o$ Replicated Cluster SAN.**
* **Justification:** Doctor lookups require low latency ($<50\,	ext{ms}$). Active patient records are cached in database shared memory (DRAM). Writes commit synchronously to NVMe SSD write-ahead logs (WAL) to ensure ACID compliance, with background asynchronous replication to the secondary hospital cluster.

### 3.3 Raw and Reconstructed MRI Scans
* **Placement:** **Direct GPU Memory $	o$ NVMe Scratch Array $	o$ Enterprise Network-Attached Storage (NAS / PACS).**
* **Justification:** Reconstructing raw k-space scanner data into 3D DICOM volumes requires multi-threaded Fast Fourier Transforms (FFT). Raw scans stream directly into GPU DRAM via PCIe DMA. Reconstructed volumes are temporarily staged on high-speed local NVMe scratch disks before archiving to the Picture Archiving and Communication System (PACS) tier.

---

## 4. Evaluation of I/O Architecture for Medical Imaging (MRI)

```
 PROGRAMMED I/O (PIO)               INTERRUPT-DRIVEN I/O               DIRECT MEMORY ACCESS (DMA)
+------------------------+         +------------------------+         +------------------------+
| CPU reads status       |         | Device fills buffer    |         | CPU programs DMA       |
| in tight busy loop.    |         | Device interrupts CPU  |         | controller (addr,count)|
| Reads each word via    |         | CPU reads word via ISR |         | DMA engine transfers   |
| CPU register.          |         | Repeats for EVERY word |         | data directly to RAM   |
+------------------------+         +------------------------+         +------------------------+
            |                                  |                                  |
            v                                  v                                  v
 100% CPU Saturation                Extreme Interrupt Latency          Zero CPU Overhead
 Completely Unusable                CPU Starvation                     Optimal for High Volume
```

1. **Programmed I/O (PIO):** The CPU executes a polling loop, reading device status flags and transferring each word through general-purpose registers. For a $500\,	ext{MB}$ MRI scan, PIO consumes 100% of CPU cycles, freezing clinical monitoring.
2. **Interrupt-Driven I/O:** The device raises a hardware interrupt when a data word/block is ready. While superior to PIO for low-speed keyboards, transferring large imaging datasets generates tens of millions of interrupts, saturating the CPU in context switching.
3. **Direct Memory Access (DMA):** The CPU configures the DMA controller with source device address, destination RAM address, and transfer size, then resumes other tasks. The DMA controller transfers data across the PCIe bus directly into physical memory, raising a **single completion interrupt** when the entire $500\,	ext{MB}$ scan is in RAM.

---

## 5. Alternative Concept and Complexity Analysis: Interrupt-Driven I/O vs DMA for a 500 MB MRI Scan

### 5.1 Mathematical Problem Formulation
An MRI scanner acquires a single examination dataset of $n = 500\,	ext{MB} = 524,288,000\,	ext{bytes}$. We compare:
* **Alternative Concept:** Interrupt-driven I/O with one interrupt generated per 64-bit word ($8\,	ext{bytes}$).
* **Baseline Concept:** Bus-mastering Direct Memory Access (DMA) transferring the entire volume with a single terminal interrupt.

### 5.2 Quantitative Evaluation

#### 1. Total Interrupt Count ($N_{int}$)
* **Interrupt-Driven I/O:**
  $$N_{int} = rac{524,288,000\,	ext{bytes}}{8\,	ext{bytes/word}} = \mathbf{65,536,000\,	ext{interrupts}}$$
* **Direct Memory Access (DMA):**
  $$N_{int} = \mathbf{1\,	ext{interrupt}} \quad (	ext{upon completion of block transfer})$$

#### 2. CPU Time Consumed ($T_{cpu}$)
Let the average interrupt service overhead (context save, ISR dispatch, ring transition, register restore) be $t_{isr} pprox 1.5\,\mu	ext{s}$:

$$T_{cpu}(	ext{Interrupt-Driven}) = 65,536,000 	imes 1.5 	imes 10^{-6}\,	ext{s} = \mathbf{98.3\,	ext{seconds of pure CPU overhead}}$$

$$T_{cpu}(	ext{DMA}) = 1 	imes 1.5 	imes 10^{-6}\,	ext{s} + t_{setup} pprox \mathbf{5.0\,\mu	ext{seconds}}$$

```
+------------------------------------+------------------------------------+
| Dimension                          | Interrupt-Driven (Word-by-Word)    | Direct Memory Access (DMA)         |
+------------------------------------+------------------------------------+
| Hardware Interrupts Generated      | 65,536,000 interrupts              | 1 completion interrupt             |
| CPU Execution Time Consumed        | 98.3 seconds (100% Core Lockup)    | 5 microseconds (<0.001%)           |
| Maximum Achievable Throughput      | Limited to ~5 MB/s by CPU trap rate| >6.5 GB/s (PCIe Gen4 x4 Bus Sat.)  |
| Memory Copy Operations             | Double copy (Device -> Reg -> RAM) | Zero copy (Device -> Bus -> RAM)   |
| System State During Transfer       | Completely unresponsive            | Fully operational for patient care |
+------------------------------------+------------------------------------+
```

### 5.3 Technical Conclusion
Using interrupt-driven I/O for high-resolution medical imaging causes immediate CPU starvation and delays image acquisition by over $1.5\,	ext{minutes}$. **Direct Memory Access (DMA) is mathematically and architecturally mandatory** for modern hospital medical imaging systems.
