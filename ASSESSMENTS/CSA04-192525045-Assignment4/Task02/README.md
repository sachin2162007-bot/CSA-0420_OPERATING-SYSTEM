# Task 02: From Workload to File-Allocation Method

**Course:** CSA04 — Operating Systems  
**Assignment:** Assessment 4 (CO4) — Optimising File Storage and Disk Access  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Degree:** B.Tech Artificial Intelligence and Machine Learning  
**Institution:** SIMATS Engineering  
**Bloom's Level:** K5 – Evaluate  
**Official Marks:** 8 Marks  

---

## 1. Task Title
**Workload-Driven File Allocation, Inode Traversal Sizing, and Free-Space Management for Video Surveillance.**

---

## 2. Official Marks
**8 Marks** (Task 2.1: 2 Marks | Task 2.2: 2 Marks | Task 2.3: 3 Marks | Task 2.4: 1 Mark)

---

## 3. Objective
To analyze the operational storage patterns of a 32-camera surveillance recorder, evaluate contiguous, linked, FAT, and indexed allocation methods, complete the official `alloc_contiguous` program, calculate inode maximum file size and traversal costs, and evaluate 4 TB free-space management techniques.

---

## 4. Problem Statement
A network video recorder (NVR) stores footage from 32 cameras. Files grow continuously, are read sequentially, are searched by time (random access), and are purged after 30 days.
- Identify operations, frequencies, and access patterns.
- Compare allocation methods and justify a recommendation.
- Calculate disk reads to reach the last block of a 4 GB file and determine max inode capacity.
- Calculate 4 TB free-space bit-vector size.
- Complete `TODO 1` in the supplied contiguous allocation C code.

---

## 5. Concepts Involved
- Contiguous, Linked, FAT, and Indexed File Allocation
- Extent-Based File Systems (XFS/ext4)
- UNIX Inode Multi-Level Pointer Architecture (Direct, Single, Double, Triple Indirect)
- Disk Traversal Mechanics & I/O Overhead
- Free-Space Management (Bit Vectors, Linked Lists, Grouping, Counting)
- External vs. Internal Storage Fragmentation

---

## 6. Design and Algorithms
- **First-Fit Contiguous Allocation:** Scans simulated disk of 32 blocks for the first run of consecutive free blocks satisfying request size.
- **Indexed Allocation:** Allocates the first free block as an index block (`e`), and collects subsequent non-contiguous free blocks as data blocks.

---

## 7. Source and Documentation Files
- `file_allocation.c`: Official C program completing `TODO 1` for first-fit contiguous allocation.
- `inode_calculator.c`: Tool calculating inode maximum file size and block traversal costs.
- `free_space_calculator.c`: Sizing tool comparing bit vectors, free lists, grouping, and counting for 4 TB drives.
- `allocation_comparison.md`: Operation table and allocation trade-off evaluation.
- `calculations.md`: Complete mathematical derivations, inode tree calculations, and PDF inconsistency analysis.
- `complexity_analysis.md`: Time and space complexity using Big-O notation.
- `Makefile`: Build rules for all C utilities.
- `run_experiments.sh`: Automated experiment script.
- `expected_output.txt`: Exact official expected terminal output.
- `inputs/allocation_test_cases.txt`: Trace inputs.

---

## 8. Compilation Commands
```bash
gcc -std=c11 -Wall -Wextra file_allocation.c -o file_allocation
gcc -std=c11 -Wall -Wextra inode_calculator.c -o inode_calculator
gcc -std=c11 -Wall -Wextra free_space_calculator.c -o free_space_calculator
```

---

## 9. Execution Commands
```bash
./file_allocation
./inode_calculator
./free_space_calculator

# Or automated execution:
chmod +x run_experiments.sh
./run_experiments.sh
```

---

## 10. Expected Results
```text
AAAAAA........CCCCC.............
E (10 blocks) contiguous: start 19
E (10 blocks) indexed: index block 6, data blocks 7 8 9 10 11 12 13 29 30 31
AAAAAAeEEEEEEECCCCCEEEEEEEEEEEEE
```

---

## 11. Calculations
- **4 GB File Blocks:** $4\text{ GiB} / 4\text{ KiB} = 1,048,576\text{ blocks} = 2^{20}\text{ blocks}$.
- **Disk Reads for Last Block:** Contiguous = 1, Linked = 1,048,576, FAT (cached) = 1, Indexed = 4.
- **Inode Max File Size:** $12 + 1024 + 1024^2 + 1024^3 = 1,074,791,436\text{ blocks} \approx \mathbf{4.004\text{ TiB}}$ ($4.402\text{ TB}$ decimal).
- **4 TB Bit-Vector Size:** $2^{30}\text{ blocks} = 2^{30}\text{ bits} = \mathbf{128\text{ MB}}$.

---

## 12. Observations & Inconsistency Resolution
Although page 35 of the PDF posits that file E (10 blocks) cannot be stored contiguously, the provided initial disk layout after releases contains a contiguous run of 13 blocks at indices 19–31. The program correctly allocates blocks 19–28, matching the official output `E (10 blocks) contiguous: start 19`.

---

## 13. Complexity Analysis
- Contiguous First-Fit: $O(M)$ time complexity, $O(1)$ space complexity.
- Indexed Allocation: $O(M)$ time complexity, $O(K)$ buffer space complexity.

---

## 14. Alternative Concept: In-Memory FAT vs. Indexed Inodes
Holding the entire FAT for a 4 TB drive in RAM requires $2^{30} \times 4\text{ B} = \mathbf{4\text{ GB RAM}}$, consuming excessive system memory. Modern extent-based indexed inodes consume negligible memory (<1 KB per active file) while maintaining direct $O(1)$ block seeking.

---

## 15. Advantages
- First-fit contiguous allocation maximizes sequential streaming read speeds.
- Indexed allocation completely eliminates external fragmentation by gathering dispersed free blocks.

---

## 16. Limitations
- Basic contiguous allocation suffers from external fragmentation and cannot dynamically expand files when neighbouring blocks are occupied.

---

## 17. Conclusion & Recommendation
For continuous 32-camera NVR recording, **Extent-Based Indexed Allocation** (as in XFS/ext4) combined with a **Bit Vector (128 MB)** is the optimal architecture.

---

## 18. Evidence Image Placeholders
- `<!-- Add Gemini screenshot here: task02_file_allocation_output.png -->`
- `<!-- Add Gemini screenshot here: task02_inode_calculation_verification.png -->`

---

## 19. References
- Silberschatz, A., Galvin, P. B., & Gagne, G. *Operating System Concepts* (10th ed.), Chapter 14: File-System Implementation.
- Linux Kernel Documentation: `Documentation/filesystems/ext4/inodes.rst`.
