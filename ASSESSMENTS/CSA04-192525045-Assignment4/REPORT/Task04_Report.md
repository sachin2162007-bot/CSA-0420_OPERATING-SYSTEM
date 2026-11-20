# Task 04: Disk Scheduling Algorithms Report

**Course**: CSA04 – Operating Systems (CO4)  
**Student**: S. Sachin (192525045)  
**Marks Allocated**: 7 Marks  

## 1. Disk Head Scheduling Evaluation
- Head starts at cylinder 53; pending requests: `98, 183, 37, 122, 14, 124, 65, 67`.
- FCFS: 640 cylinders (excessive mechanical seek thrashing).
- SSTF: 236 cylinders (minimizes immediate seek distance).
- SCAN: 236 cylinders (downward sweep to 0, upward sweep to 183).
- C-SCAN: 382 cylinders (upward sweep to 199, jump to 0, service to 37).

## 2. Completed Verification Program
- Completed `disk_scheduling.c` with TODO 1 resolved reproduces the exact expected output from the official PDF.

## 3. Starvation Evaluation and SSD Schedulers
- SSTF demonstrates severe starvation of peripheral requests (cylinder 183) under continuous localized traffic near cylinder 60.
- Linux SSD schedulers (`none`, `kyber`) bypass elevator sorting to exploit NVMe multi-queue hardware parallelism.
