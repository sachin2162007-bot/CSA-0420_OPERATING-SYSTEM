# CSA04 — Operating Systems: Assignment 1 (CO1)
## Smart Hospital Operating-System Design, Implementation & Evaluation

### Student & Repository Details
* **Student Name:** S. Sachin
* **Register Number:** 192525045
* **Course:** CSA04 – Operating Systems (CO1)
* **Degree:** Bachelor of Technology
* **Repository:** `CSA-0420_OPERATING-SYSTEM` / `CSA04-192525045-Assignment1`
* **Assignment Title:** Evaluate and Justify the Operating-System Design of a Smart Hospital
* **Marks Assessed:** 50 Marks (Bloom's Taxonomy Level: K5 – Evaluate)

---

## 1. Directory Structure

```
CSA04-192525045-Assignment1/
├── README.md                                  # Complete repository documentation
├── EVIDENCE_STATUS.txt                        # Formal evidence generation tracking report
├── run_all_evidence.sh                        # Automated master execution test suite
├── Report/                                    # Academic reports and assignment briefs
│   └── assignment_co1.docx                    # Original assignment specification document
├── Task01/                                    # Requirements Analysis
│   ├── task_01_requirements_analysis.md      # Detailed evaluation across 6 computing environments
│   └── task_01_requirements_table.md         # Ranked requirements matrix and justifications
├── Task02/                                    # Operating-System Types
│   ├── task_02_os_types_analysis.md          # Real-time, distributed, clustered, and mobile OS evaluation
│   └── task_02_os_types_comparison.md        # Comprehensive OS type comparison matrix
├── Task03/                                    # Operating-System Structures
│   ├── task_03_os_structures_analysis.md     # Monolithic, microkernel, layered, and hybrid structures
│   └── task_03_os_structures_comparison.md   # Structural trade-off matrix and architectural defense
├── Task04/                                    # Operating-System Services
│   └── task_04_os_services.md                # 10 core OS services, service matrix, tablet OS analysis
├── Task05/                                    # System Calls Examination
│   ├── task5_syscall_demo.c                  # C program demonstrating 6 categories of system calls
│   ├── copy_raw.c                            # Raw POSIX open/read/write/close file copier
│   ├── copy_stdio.c                          # Standard C library fopen/fread/fwrite/fclose copier
│   ├── run_task5.sh                          # Automated compilation and strace execution script
│   ├── task_05_system_calls.md               # Step-by-step read() trace, registers, strace mechanics
│   └── evidence/                             # Execution evidence directory (Linux-generated outputs)
├── Task06/                                    # Buffer Size Experiment
│   ├── task6.c                               # Completed code (TODO 1) evaluating buffer read() counts
│   ├── copy_raw_bench.c                      # High-resolution benchmark for raw system call copying
│   ├── copy_stdio_bench.c                    # High-resolution benchmark for stdio library copying
│   ├── copy_mmap_bench.c                     # High-resolution benchmark for memory-mapped (mmap) copying
│   ├── run_task6.sh                          # Benchmark sweep across buffer sizes (1B to 1MB) & strace
│   ├── task_06_buffer_size_experiment.md     # Mathematical buffer model, mmap analysis, backup recommendation
│   └── evidence/                             # Execution evidence directory (Linux-generated outputs)
├── Task07/                                    # Process Control System Calls
│   ├── task7.c                               # Completed shell job execution program (TODO 1)
│   ├── task7.cpp                             # Preserved original C++ source
│   ├── task7_experiment.c                    # Experiment demonstrating stdio file redirection duplication
│   ├── spawn_bench_1000.c                    # Benchmark measuring 1,000 process spawns (3 runs + median)
│   ├── run_task7.sh                          # Compilation and execution script for Task 7 evidence
│   ├── task_07_process_control.md            # fork/exec/wait/exit lifecycle, zombies, orphans, stdio bug
│   └── evidence/                             # Execution evidence directory (Linux-generated outputs)
├── Task08/                                    # CPU States and Hardware Protection
│   └── task_08_cpu_states_protection.md      # Dual-mode, privileged ops, timer interrupt, MMU, polling
├── Task09/                                    # Memory Hierarchy and I/O Systems
│   └── task_09_memory_io_systems.md          # Memory latencies, hospital data placement, DMA vs Interrupts
├── Task10/                                    # Performance Measurement
│   ├── proc_vs_thread.c                      # 1,000 processes vs 1,000 threads benchmark (3 runs + median)
│   ├── run_task10.sh                         # Compilation and GNU time -v execution script
│   ├── task_10_performance_measurement.md    # Process vs thread memory, context switch, and CPU analysis
│   └── evidence/                             # Execution evidence directory (Linux-generated outputs)
├── Task11/                                    # Final Architecture Recommendation & Defense
│   └── task_11_recommendation_defense.md     # Defended OS plan for 6 environments, scaling, virtualization
└── Task12/                                    # Day-4 Design Challenge: Home Monitoring & Cloud Records
    └── task_12_day4_design_challenge.md      # Impact analysis, evaluation, edge-to-cloud adaptation, defense
```

---

## 2. Compilation and Execution Guide (Linux Environment)

### Prerequisites
* **Operating System:** Ubuntu 22.04 LTS / Debian 12 / Fedora 39 / RHEL 9 (or WSL2 with Linux kernel)
* **Compiler:** `gcc` (GCC 11, 12, or 13 supporting C11 standard)
* **System Utilities:** `strace`, `/usr/bin/time`, `python3`, `bash`

```bash
# Ubuntu / Debian installation of required toolchain
sudo apt-get update
sudo apt-get install -y build-essential gcc strace time python3
```

### Exact Compilation Commands

All C programs conform strictly to the **C11 standard** with full diagnostic warnings:

```bash
# Task 05: System Calls
gcc -std=c11 -Wall -Wextra Task05/task5_syscall_demo.c -o Task05/task5_syscall_demo
gcc -std=c11 -Wall -Wextra Task05/copy_raw.c -o Task05/copy_raw
gcc -std=c11 -Wall -Wextra Task05/copy_stdio.c -o Task05/copy_stdio

# Task 06: Buffer Sizing Experiment (with -O2 optimization for benchmarks)
gcc -std=c11 -Wall -Wextra Task06/task6.c -o Task06/task6
gcc -std=c11 -Wall -Wextra -O2 Task06/copy_raw_bench.c -o Task06/copy_raw_bench
gcc -std=c11 -Wall -Wextra -O2 Task06/copy_stdio_bench.c -o Task06/copy_stdio_bench
gcc -std=c11 -Wall -Wextra -O2 Task06/copy_mmap_bench.c -o Task06/copy_mmap_bench

# Task 07: Process Control
gcc -std=c11 -Wall -Wextra Task07/task7.c -o Task07/task7
gcc -std=c11 -Wall -Wextra Task07/task7_experiment.c -o Task07/task7_experiment
gcc -std=c11 -Wall -Wextra -O2 Task07/spawn_bench_1000.c -o Task07/spawn_bench_1000

# Task 10: Process vs Thread Performance (-pthread required)
gcc -std=c11 -Wall -Wextra -pthread -O2 Task10/proc_vs_thread.c -o Task10/proc_vs_thread
```

### Automated Master Execution

To run the complete automated test suite and generate all required TXT evidence files:

```bash
# Grant execution permissions
chmod +x run_all_evidence.sh Task05/*.sh Task06/*.sh Task07/*.sh Task10/*.sh

# Run master evidence collection
./run_all_evidence.sh
```

---

## 3. Short Note of Results for Each Task

* **Task 01 (Requirements):** Established that bedside monitors require hard real-time response ($\le 10\,	ext{ms}$) while database servers prioritize zero-downtime transactional durability. Confirmed that deploying a single GPOS everywhere violates clinical safety.
* **Task 02 (OS Types):** Selected Microkernel RTOS for monitors, Clustered Linux with pacemaker failover for the database, SMP Linux with CUDA for the imaging server, and hardened mobile OS for nurse tablets.
* **Task 03 (OS Structures):** Defended a Microkernel structure (QNX) for monitors to isolate device driver crashes from core alarm routines, and a modular monolithic kernel (Linux) for high-performance database I/O.
* **Task 04 (OS Services):** Stripped dynamic ELF loading, virtual memory swapping, and hierarchical file systems from bedside monitors to guarantee bounded latency. Rejected full desktop OSs on tablets due to 4.5-hour battery exhaustion.
* **Task 05 (System Calls):** Implemented C demonstrations of 6 syscall categories. Traced `read()` through x86-64 ABI registers (`%rax=0`, `%rdi`, `%rsi`, `%rdx`), MSR_LSTAR trap, privilege switch (Ring 3 $	o$ 0), VFS dispatch, and `sysretq`.
* **Task 06 (Buffer Sizing):** Completed TODO 1. Proved that increasing buffer size from 1B to 4096B reduces system calls by $99.96\%$ (from 10,001 to 4 read calls on a 10KB file). Recommended 64KB/128KB buffer for hospital database backups.
* **Task 07 (Process Control):** Implemented shell process control with `fork()`, `execlp()`, and `waitpid()`. Uncovered and explained the stdio buffer duplication bug occurring when output is redirected to a file without `fflush(stdout)`.
* **Task 08 (CPU States & Protection):** Evaluated dual-mode hardware enforcement, mode bit toggling, and timer preemption. Proved that unprivileged interrupt disabling (`cli`) leads to patient mortality during cardiac events.
* **Task 09 (Memory & I/O):** Structured data placement across storage tiers. Proved that transferring a 500 MB MRI scan via word-by-word interrupt I/O requires 65,536,000 interrupts and 98.3 seconds of CPU time, proving DMA is mandatory.
* **Task 10 (Performance Measurement):** Benchmarked 1,000 processes versus 1,000 threads. Threads proved $12	imes - 18	imes$ faster due to shared virtual address space (`mm_struct`) and elimination of full TLB invalidations.
* **Task 11 (OS Recommendation):** Synthesized the complete hospital OS blueprint across all 6 environments, defended scalability to 3 hospitals, and refuted full-system virtualization for bedside monitoring.
* **Task 12 (Design Challenge):** Adapted the architecture for home patient monitoring and cloud-shared records. Defended local edge RTOS alarm generation ($<5\,	ext{ms}$) to bypass Internet latency, combined with TLS 1.3 MQTT store-and-forward queuing and zero-trust cloud encryption.
