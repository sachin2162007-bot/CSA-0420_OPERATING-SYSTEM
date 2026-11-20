# Task 04: Disk Scheduling Algorithms

**Course**: CSA04 – Operating Systems  
**Student Name**: S. Sachin  
**Register Number**: 192525045  
**Course Outcome**: CO4 (Bloom's Level K5 – Evaluate)  
**Institution**: SIMATS Engineering  

---

## 1. Overview and Problem Statement
In secondary storage management, mechanical hard disk drives (HDDs) incur significant seek latency as the physical read/write head moves across magnetic platters. Disk scheduling algorithms reorder pending I/O requests to minimize total head movement, reduce average seek time, and balance fairness against starvation.

This task examines a 200-cylinder disk ($0$ to $199$) with the head initially at cylinder $53$ and pending requests:
$$[98, 183, 37, 122, 14, 124, 65, 67]$$

---

## 2. Official Deliverables
1. `disk_scheduling.c`: Completed C implementation with TODO 1 (SSTF) resolved, reproducing exact PDF outputs.
2. `disk_scheduling_simulator.c`: Full-scale benchmark of 6 algorithms across 10,000 requests.
3. `ssd_scheduler_check.sh`: Production shell script for inspecting Linux block device schedulers.
4. `disk_scheduler_analysis.md`: In-depth comparative evaluation of FCFS, SSTF, SCAN, C-SCAN, LOOK, and C-LOOK.
5. `fairness_analysis.md`: Mathematical and practical starvation analysis of SSTF vs elevator algorithms.
6. `calculations.md`: Complete step-by-step arithmetic traces for all algorithms.
7. `complexity_analysis.md`: Time and space complexity models ($O(N)$, $O(N \log N)$, $O(N^2)$).
8. `expected_output.txt`: Verifiable output log matching assignment requirements.

---

## 3. Official Verification Output
```
FCFS Total Head Movement : 640
SSTF Total Head Movement : 236
SCAN Total Head Movement : 236
C-SCAN Total Head Movement: 382
```

---

## 4. Compilation and Execution
```bash
# Build all binaries
make all

# Run official evaluation
./disk_scheduling

# Run 10,000-request benchmark and starvation demonstration
./disk_scheduling_simulator

# Execute all experiments
bash run_experiments.sh
```

---

## 5. Architectural Comparison Summary
- **FCFS** ($640$ cyls): Purely fair, but incurs severe head thrashing.
- **SSTF** ($236$ cyls): Minimizes immediate seek, but starves peripheral requests under heavy load.
- **SCAN** ($236$ cyls): Sweeps toward edge, eliminating starvation at slight expense of turn-around variance.
- **C-SCAN** ($382$ cyls): Provides uniform waiting time distribution across all cylinder tracks.
- **LOOK / C-LOOK**: Enhances SCAN by reversing at the outermost request rather than the disk physical boundary.
- **Solid-State Drives**: Possess zero mechanical seek latency; Linux utilizes multi-queue bypass (`none`) or latency-based schedulers (`kyber`).

<!-- Add Gemini screenshot here: Disk Scheduling Gantt chart and seek traces -->
