# Task 06: Disk Management and Storage Subsystems

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assessment 3 (CO3)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Bloom's Level:** K5 – Evaluate  
**Problem Context:** High-Availability Storage Design for a Hospital Records Server  

---

## 1. Task Title
**Evaluation of Disk Formatting, Boot Blocks, Swap Architecture, and RAID Reliability.**

---

## 2. Problem Context
A hospital electronic health records (EHR) server must provide non-stop 24/7 reliability, survive disk hardware failures without patient data loss, maintain sufficient swap space during heavy query surges, and deliver fast response times for emergency patient lookups.

---

## 3. Objective
To design, implement, and evaluate:
1. Low-level formatting, partitioning (MBR vs GPT), and logical formatting (filesystems).
2. Boot block sequence (UEFI $\to$ GRUB $\to$ Linux Kernel $\to$ systemd).
3. Bad-block remapping and sector sparing mechanisms.
4. Swap partition sizing and placement (Dedicated disk vs. swap file).
5. Usable storage capacity calculations for 6 disks $\times$ 4 TB across RAID 0, 1, 5, 6, and 10.
6. Mathematical modeling of data loss probability during disk array rebuilds.
7. Safe, non-destructive read performance benchmarking.

---

## 4. Concepts Involved
- **Low-Level Formatting:** Physical creation of sectors, preambles, data fields, and Error-Correcting Code (ECC) by the manufacturer.
- **Partitioning:** Dividing the raw storage unit into logical slices (GPT partition tables support up to 128 partitions with 64-bit LBA).
- **Logical Formatting:** Writing filesystem data structures (superblock, inode tables, allocation bitmaps, journal).
- **Boot Process:**
  1. System power-on triggers BIOS/UEFI firmware execution.
  2. Boot block read from Master Boot Record (MBR) or EFI System Partition (ESP).
  3. Primary boot loader (GRUB) loaded into memory $\to$ presents OS menu.
  4. Kernel image (`vmlinuz`) and initial RAM disk (`initramfs`) unpacked.
  5. Kernel initializes drivers, mounts root filesystem `/`, and launches PID 1 (`systemd`).
- **Bad-Block Management:** Disk controller automatically transparently maps defective sectors to reserved spare sectors (sector sparing / slipping).
- **RAID Levels (Redundant Array of Independent Disks):**
  - *RAID 0:* Pure striping (High speed, zero redundancy).
  - *RAID 1:* Mirroring (100% redundancy, fast reads, safe).
  - *RAID 5:* Striping with single distributed parity (Good capacity, $4\times$ write penalty).
  - *RAID 6:* Striping with dual distributed parity (Survives 2 disk failures, $6\times$ write penalty).
  - *RAID 10:* Striped mirrors $1+0$ (High performance, $2\times$ write penalty, rapid rebuild).

---

## 5. Files Created
- `raid_capacity.c`: Usable capacity and efficiency calculator for 6 disks $\times$ 4 TB.
- `raid_reliability.c`: Mathematical probability modeler for array failure during rebuilds.
- `disk_information.sh`: Safe non-destructive Linux partition and swap inspector (`lsblk`, `swapon`).
- `disk_benchmark.sh`: Safe non-destructive 64 MB buffered vs direct read speed evaluator.
- `run_experiments.sh`: Master experiment orchestration script.
- `calculations.txt`: Step-by-step mathematical proofs for RAID capacity, rebuild failure probabilities, and write penalties.
- `results.txt`: Comprehensive output verification and engineering recommendations.

---

## 6. System Requirements
- Ubuntu Linux (x86_64)
- GCC C11 Compiler (`-std=c11 -Wall -Wextra -lm`)
- POSIX utilities (`lsblk`, `swapon`, `dd`)

---

## 7. Compilation Commands
```bash
gcc -std=c11 -Wall -Wextra raid_capacity.c -o raid_capacity
gcc -std=c11 -Wall -Wextra raid_reliability.c -o raid_reliability -lm
```

---

## 8. Execution Commands
```bash
./raid_capacity
./raid_reliability
bash disk_information.sh
bash disk_benchmark.sh

# Or automated execution:
chmod +x run_experiments.sh
./run_experiments.sh
```

---

## 9. Expected Output (RAID 6-Disk x 4 TB Evaluation)
```text
===================================================================================
  RAID USABLE CAPACITY AND EFFICIENCY EVALUATOR                                    
  Student: S. Sachin | Reg: 192525045 | CSA04 CO3                                  
  Disks: 6 drives | Individual Drive Capacity: 4.0 TB | Raw Storage: 24.0 TB      
===================================================================================
RAID Level | Usable (TB)  | Parity (TB)  | Efficiency (%) | Fault Tol    | Write Penalty
-----------------------------------------------------------------------------------
RAID 0     |    24.0 TB   |      0.0 TB  |        100.0%  | 0 Disks      | 1x (No overhead)
RAID 1     |    12.0 TB   |     12.0 TB  |         50.0%  | 1 / pair (3) | 2x (Dual write)
RAID 5     |    20.0 TB   |      4.0 TB  |         83.3%  | 1 Disk       | 4x (2 Read, 2 Write)
RAID 6     |    16.0 TB   |      8.0 TB  |         66.7%  | 2 Disks      | 6x (3 Read, 3 Write)
RAID 10    |    12.0 TB   |     12.0 TB  |         50.0%  | 1 / pair (3) | 2x (Dual write)
```

---

## 10. Illustrative Sample Output (Reliability Modeler)
```text
RAID Level | Data Loss Probability (P_loss) | Relative Risk Assessment
-------------------------------------------------------------------------------
RAID 5     |           0.00041088 (0.0411%) | Baseline High Risk (1.0x)
RAID 10    |           0.00008219 (0.0082%) | 5.0x Safer than RAID 5
RAID 6     |           0.00000007 (0.0000%) | 6087x Safer than RAID 5
```

---

## 11. Calculations
- **Usable Capacity Formulas:**
  - $\text{RAID 0} = N \times C = 24\text{ TB}$
  - $\text{RAID 1 / 10} = (N / 2) \times C = 12\text{ TB}$
  - $\text{RAID 5} = (N - 1) \times C = 20\text{ TB}$
  - $\text{RAID 6} = (N - 2) \times C = 16\text{ TB}$
- **Rebuild Risk Derivations:**
  $$P_{\text{loss}}(\text{RAID 5}) = 1 - (1 - p_{\text{window}})^5 \approx 0.0411\%$$
  $$P_{\text{loss}}(\text{RAID 10}) = p_{\text{window}} \approx 0.0082\% \implies 5\times \text{ safer}$$
  $$P_{\text{loss}}(\text{RAID 6}) \approx 10 \times (p_{\text{window}})^2 \approx 6.75 \times 10^{-8} \implies 6,000\times \text{ safer}$$

---

## 12. Experimental Investigation
1. **Capacity Scaling:** Modeled exact capacity and parity distribution across all 5 primary RAID levels.
2. **Rebuild Risk Modeling:** Proved that on 4 TB drives requiring a 24-hour rebuild window, RAID 5 exposes the hospital to unacceptable risk due to Unrecoverable Read Errors (URE).
3. **Storage Performance:** Safe 64 MB non-destructive read benchmarking demonstrated that NVMe SSDs outperform HDDs by over $20\times$ in throughput and $100\times$ in latency.

---

## 13. Evaluation: Swap Partition vs. Swap File
- **Dedicated Swap Partition:** 
  - *Pros:* Contiguous LBA blocks; bypasses filesystem overhead; optimal I/O throughput.
  - *Cons:* Fixed partition size; difficult to expand without repartitioning.
- **Swap File (`/swapfile`):** 
  - *Pros:* Dynamically expandable at runtime (`dd` + `mkswap` + `swapon`).
  - *Cons:* Slight block indirection overhead; susceptible to fragmentation over time.

---

## 14. Alternative Concept: RAID 6 vs. RAID 10
- **RAID 6:** Uses dual distributed parity ($P+Q$). Can survive TWO simultaneous disk failures. Usable capacity is 16 TB (66.7%), but write operations incur a $6\times$ write penalty (read data, read P, read Q, write data, write P, write Q). Ideal for write-infrequent medical image archives.
- **RAID 10:** Uses striped mirroring. Write penalty is only $2\times$ and rebuild is a simple drive-to-drive mirror copy without CPU parity calculations. Ideal for write-intensive database transactions.

---

## 15. Time Complexity
- **RAID 0 Read/Write:** $O(1)$ block direct access.
- **RAID 5 Random Write:** $O(1)$ algorithmically, but requires 4 physical disk seeks.
- **RAID 6 Random Write:** $O(1)$ algorithmically, but requires 6 physical disk seeks and Galois Field $GF(2^8)$ parity math.

---

## 16. Space Complexity
- Space efficiency $\eta = \frac{N - k}{N}$ where $k$ is parity disk count ($k=0$ for RAID 0, $k=1$ for RAID 5, $k=2$ for RAID 6, $k=N/2$ for RAID 1/10).

---

## 17. Advantages & Limitations
- **RAID 10:** Blazing IOPS, rapid rebuild, but 50% capacity loss.
- **RAID 5:** High capacity (83%), but slow rebuild and vulnerable to secondary failure.
- **RAID 6:** Outstanding safety (survives 2 failures), but high write overhead.

---

## 18. Conclusion & Design Recommendation
For the hospital records server, a **Tiered Storage Architecture** is recommended:
1. **Transactional EHR Database & OS:** **RAID 10 (12 TB usable)** on enterprise SSDs. Eliminates parity write penalties, ensures high IOPS for concurrent doctor queries, and provides fast 5x safer rebuilds.
2. **Medical Imaging (PACS CT/MRI Scans):** **RAID 6 (16 TB usable)** on high-capacity enterprise HDDs. Protects irreplaceable patient scans with dual-drive fault tolerance while delivering 66.7% capacity efficiency.
3. **Swap Allocation:** A dedicated **64 GB Swap Partition** placed on a separate high-speed NVMe mirror, ensuring memory surges never stall emergency room workstations.

---

## 19. Image Placeholders
- `[INSERT IMAGE: raid_architecture.png]`
