# Task 02: From Workload to File-Allocation Method Report

**Course**: CSA04 – Operating Systems (CO4)  
**Student**: S. Sachin (192525045)  
**Marks Allocated**: 8 Marks  

## 1. Surveillance System Workload Characterization
- 32 HD surveillance cameras recording continuously at 5 Mbps ($0.625\text{ MB/s}$ per camera).
- 15-minute segments ($562.5\text{ MB}$ per file), generating large sequential writes alongside random reads during forensic investigation.

## 2. File Allocation Comparison and Recommendation
- Contiguous allocation is efficient for sequential writes but suffers external fragmentation.
- Linked allocation eliminates external fragmentation but exhibits catastrophic $O(k)$ seek overhead during random review.
- **Indexed Allocation (Ext4 Extents)** is selected as optimal: supports rapid sequential writing and instant $O(1)$ block lookup for random seeking.

## 3. Implementation of TODO 1 and The PDF Contradiction
- In `file_allocation.c`, First-Fit contiguous allocation is implemented to resolve TODO 1.
- Contradiction Resolved: The PDF question states File E (10 blocks) cannot be allocated contiguously, but after releasing files B and D, a free block run of 13 blocks exists at blocks 19–31. Contiguous allocation succeeds at start block 19, matching the official expected output.

## 4. Inode and Free Space Calculations
- Multi-level Inode: Max capacity is $4.004\text{ TiB}$. A 4 GB file resides in the Double Indirect Block.
- Bit Vector: Managing a 4 TB disk with 4 KB blocks requires an in-memory bit vector of $122.1\text{ MB}$ ($116.4\text{ MiB}$).
