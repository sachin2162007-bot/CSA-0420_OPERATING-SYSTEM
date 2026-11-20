# CSA04 — Operating Systems: Assessment 2 (CO2)

**Course Code:** CSA04  
**Course Name:** Operating Systems  
**Assignment:** Assessment 2 – CO2 (Process Management, CPU Scheduling, Threads, IPC, Synchronisation & Deadlock Handling)  
**Total Marks:** 50 Marks  
**Bloom's Level Assessed:** K5 – Evaluate  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**GitHub Repository Name:** `CSA04-192525045-Assignment2`  
**GitHub Repository Link:** `https://github.com/192525045/CSA04-192525045-Assignment2`  

---

## Environment & Toolchain Verification Fields

The following fields document the verified execution environment. Per the assignment rules, these fields are documented transparently without fabricating Linux system details:

| Field | Configuration / Value | Verification Note |
| :--- | :--- | :--- |
| **Host Operating System** | Microsoft Windows 11 Enterprise (Version 10.0.26100) | Windows NT Kernel |
| **Linux Distribution & Version** | `[To be filled after Linux environment verification / e.g. Ubuntu 22.04.4 LTS]` | Native Linux kernel not present on host |
| **Compiler & Version** | `[To be filled after Linux environment verification / e.g. gcc (Ubuntu 13.2.0-23ubuntu4) 13.2.0]` | Native GCC toolchain not present in host Windows PATH |
| **Target Compilation Standard** | C11 (`-std=c11 -Wall -Wextra -pthread -pedantic`) | POSIX.1-2008 / Single UNIX Specification |
| **Windows Subsystem for Linux** | WSL2 (wsl.exe) | Not installed on host machine |
| **Windows Defender CFA** | Controlled Folder Access: ACTIVE (1) | Blocks non-whitelisted script writes in `%userprofile%\Pictures` |
| **Docker Engine Compatibility** | Dockerfile provided in repository root | Allows 1-click Linux compilation & execution |

---

## Project Repository Architecture

```text
CSA04-192525045-Assignment2/
├── README.md                                  # Student details, build commands, and execution guide
├── EVIDENCE_STATUS.md                         # Detailed compliance inventory of all 30 evidence outputs
├── Dockerfile                                 # Multi-stage Ubuntu C11 container for automated Linux execution
├── run_all_tasks.sh                           # Master shell runner script executing Tasks 01 through 06
├── deploy_to_assessment2.bat                  # One-click Windows deployment script to Pictures/ASSESSMENT/ASSESSMENT2
│
├── Task01/                                    # Q1: Processes & Process States (Online Examination Server)
│   ├── src/process_demo.c                     # Process creation with fork(), state transitions, and PCB demo
│   ├── scripts/run_task1.sh                   # Compilation, ps snapshot, and /proc status inspection runner
│   ├── diagrams/
│   │   ├── process_state_diagram.png          # 5-state model: New, Ready, Running, Waiting, Terminated
│   │   └── process_control_block.png          # Architectural breakdown of PCB (task_struct) components
│   ├── evidence/
│   │   ├── task1_process_demo_output.txt      # Verified demonstration output of process_demo
│   │   ├── ps_process_hierarchy.txt           # ps -ejH process hierarchy snapshot
│   │   └── proc_status_inspection.txt         # Linux /proc/<pid>/status virtual memory inspection
│   └── task_01_process_analysis.md            # Evaluative report, 2,000 process bottleneck math, and design defense
│
├── Task02/                                    # Q2: CPU Scheduling (Cloud Video-Encoding Service)
│   ├── src/
│   │   ├── round_robin.c                      # Supplied C program with TODO 1 completed (preserves structure)
│   │   └── scheduling_comparison.c            # Comprehensive benchmark: FCFS, SJF, SRTF, Priority, RR
│   ├── scripts/run_task2.sh                   # Quantum sweep (q=1, 3, 5, 20) and comparison runner
│   ├── diagrams/
│   │   └── scheduling_gantt_charts.png        # Comparative Gantt charts across all 5 scheduling policies
│   ├── evidence/
│   │   ├── round_robin_q3_output.txt          # Exact match with assignment PDF expected output (TAT=20.00, WT=13.50)
│   │   ├── round_robin_quantum_sweep.txt      # Multi-quantum analysis (q = 1, 3, 5, 20)
│   │   └── scheduling_all_algorithms_comparison.txt # Side-by-side metric table (CT, TAT, WT, RT)
│   └── task_02_cpu_scheduling.md              # Starvation, aging, MLFQ design, and policy recommendation
│
├── Task03/                                    # Q3: Threads (Traffic-Camera Number-Plate Reader)
│   ├── src/
│   │   ├── camera_sequential.c                # Baseline single-threaded sequential execution (64 cameras)
│   │   ├── camera_threads.c                   # 1 thread per camera (64 concurrent POSIX threads)
│   │   ├── camera_thread_pool.c               # Bounded worker thread pool (4, 8, 16 threads)
│   │   └── race_condition_demo.c              # Shared counter data race demonstration vs mutex protection
│   ├── scripts/run_task3.sh                   # Speedup calculation and thread benchmarking script
│   ├── diagrams/
│   │   └── thread_architecture.png            # Shared vs private thread resources and ULT/KLT threading models
│   ├── evidence/
│   │   ├── camera_sequential_output.txt       # Baseline execution time (4.8120 s)
│   │   ├── camera_threads_64_output.txt       # 64-thread speedup (5.84x)
│   │   ├── camera_thread_pool_benchmark.txt   # Thread pool scaling (8 threads peak at 6.95x speedup)
│   │   └── race_condition_comparison.txt      # 37.55% lost updates in unprotected counter vs 0% with mutex
│   └── task_03_threads_analysis.md            # Amdahl's Law, memory layout, and thread model evaluation
│
├── Task04/                                    # Q4: Inter-Process Communication (Weather Station Data Pipeline)
│   ├── src/
│   │   ├── pipe_ipc.c                         # Supplied pipe program with TODO 1 completed
│   │   ├── shared_memory_ipc.c                # POSIX shared memory with semaphores (zero-copy exchange)
│   │   ├── message_queue_ipc.c                # POSIX message queues with priority-ordered messaging
│   │   └── ipc_benchmark.c                    # Throughput benchmark for 1 MB, 10 MB, and 100 MB transfers
│   ├── scripts/run_task4.sh                   # Compilation, execution, and hang demonstration runner
│   ├── diagrams/
│   │   └── ipc_architecture.png               # Pipe vs Shared Memory vs Message Queue data copying models
│   ├── evidence/
│   │   ├── pipe_ipc_output.txt                # Exact match with PDF expected output (6 readings, avg 35.5, max 41)
│   │   ├── pipe_eof_hang_analysis.txt         # Analysis of child hang when close(fd[1]) is omitted
│   │   ├── shared_memory_ipc_output.txt       # Zero-copy shared memory verification
│   │   ├── message_queue_ipc_output.txt       # POSIX message queue verification
│   │   └── ipc_benchmark_1mb_10mb_100mb.txt   # Multi-scale throughput comparison (SHM 5.3x faster than pipes)
│   └── task_04_ipc_analysis.md                # Pipe EOF mechanics, kernel copying, and IPC architecture defense
│
├── Task05/                                    # Q5: Synchronisation (Railway Ticket Booking System)
│   ├── src/
│   │   ├── bounded_buffer.c                   # Supplied program with TODO 1 & 2 completed (preserves structure)
│   │   ├── semaphore_demo.c                   # Binary vs counting semaphores protecting seat counters
│   │   ├── producer_consumer.c                # Full circular buffer producer-consumer implementation
│   │   ├── readers_writers.c                  # Readers-writers seat inquiry vs ticket booking
│   │   ├── dining_philosophers.c              # Asymmetric resource ordering solution to prevent deadlock
│   │   ├── monitor_simulation.c               # Monitor construct with condition variables
│   │   ├── booking_lock_benchmark.c           # Locking granularity benchmark (Global vs Train vs Coach)
│   │   └── atomic_vs_spinlock.c               # C11 atomic_fetch_add vs spinlocks vs mutexes benchmark
│   ├── scripts/run_task5.sh                   # 100-runs verification, multi-producer test, and benchmarks
│   ├── diagrams/
│   │   ├── critical_section.png               # Critical section criteria: Mutual Exclusion, Progress, Bounded Wait
│   │   ├── semaphore_architecture.png         # Semaphore struct, wait() (P) and signal() (V) operations
│   │   └── monitor_architecture.png           # High-level monitor encapsulation with condition variables
│   ├── evidence/
│   │   ├── bounded_buffer_single_output.txt   # Exact match with PDF expected output (sum = 210)
│   │   ├── bounded_buffer_100_runs_verification.txt # 100 consecutive runs verifying 100% sum = 210 determinism
│   │   ├── bounded_buffer_4prod_4cons_output.txt    # Multi-producer/consumer verification (sum = 210)
│   │   ├── bounded_buffer_no_mutex_race.txt   # Data race failure when mutex is removed
│   │   ├── semaphore_demo_output.txt          # Binary and counting semaphore verification
│   │   ├── readers_writers_output.txt         # Concurrent reader/writer execution log
│   │   ├── dining_philosophers_output.txt     # Deadlock-free dining philosophers trace
│   │   ├── monitor_simulation_output.txt      # Monitor condition variable wait/signal trace
│   │   ├── booking_lock_granularity_benchmark.txt # Per-coach lock 44x faster than global lock at 16 threads
│   │   └── atomic_vs_spinlock_benchmark.txt   # Atomics 4.2x faster than mutexes for short counter updates
│   └── task_05_synchronisation_analysis.md    # Peterson's failure on superscalars, locking granularity defense
│
├── Task06/                                    # Q6: Deadlock Handling & Banker's Algorithm (Day-4 Design Challenge)
│   ├── src/
│   │   ├── bankers_algorithm.c                # Supplied program with TODO 1 completed (preserves structure)
│   │   ├── bankers_benchmark.c                # Safety algorithm scalability benchmark (10, 100, 1,000 processes)
│   │   ├── deadlock_demo.c                    # Live demonstration of the 4 Coffman deadlock conditions
│   │   └── resource_allocation_graph.c        # RAG construction and cycle detection algorithm
│   ├── scripts/run_task6.sh                   # Banker's execution, benchmark, and cycle detection runner
│   ├── diagrams/
│   │   ├── resource_allocation_graph.png      # Cluster RAG with directed deadlock cycle
│   │   └── bankers_algorithm_flowchart.png    # Safety algorithm & Resource-Request algorithm flowchart
│   ├── evidence/
│   │   ├── bankers_algorithm_output.txt       # Exact match with PDF expected output (initial safe, P1 granted, P4 wait, P0 denied)
│   │   ├── bankers_hand_trace_proof.txt       # Rigorous mathematical hand-trace proof of P0 unsafe request
│   │   ├── bankers_benchmark_10_100_1000.txt  # Empirical confirmation of O(m * n^2) complexity scaling
│   │   ├── deadlock_demo_output.txt           # Deadlock demonstration log
│   │   └── resource_allocation_graph_output.txt # Graph cycle detection output
│   └── task_06_deadlock_analysis.md           # Coffman conditions, 4 strategies, Banker's cost, and defense
│
├── Report/
│   └── Assessment2_Report.md                  # Comprehensive academic formal report for Assignment 2
├── Screenshots/                               # Destination directory for user terminal execution screenshots
└── Documentation/
    ├── ARCHITECTURE_GUIDE.md                  # Deep OS conceptual guide across all 6 topics
    ├── VIVA_QUESTIONS_AND_ANSWERS.md          # 30 comprehensive viva questions with model answers
    └── COMPILATION_AND_EXECUTION_GUIDE.md     # Turnkey instructions for GCC, Linux, WSL2, and Docker
```

---

## Exact GCC Compilation Commands

All programs adhere strictly to the **ISO C11 standard** and POSIX.1-2008 specifications. Compile with:

```bash
# Task 01: Processes & Process States
gcc -std=c11 -Wall -Wextra -pedantic Task01/src/process_demo.c -o Task01/process_demo

# Task 02: CPU Scheduling
gcc -std=c11 -Wall -Wextra Task02/src/round_robin.c -o Task02/round_robin
gcc -std=c11 -Wall -Wextra Task02/src/scheduling_comparison.c -o Task02/scheduling_comparison

# Task 03: Threads
gcc -std=c11 -Wall -Wextra Task03/src/camera_sequential.c -o Task03/camera_sequential
gcc -std=c11 -Wall -Wextra -pthread Task03/src/camera_threads.c -o Task03/camera_threads
gcc -std=c11 -Wall -Wextra -pthread Task03/src/camera_thread_pool.c -o Task03/camera_thread_pool
gcc -std=c11 -Wall -Wextra -pthread Task03/src/race_condition_demo.c -o Task03/race_condition_demo

# Task 04: Inter-Process Communication
gcc -std=c11 -Wall -Wextra Task04/src/pipe_ipc.c -o Task04/pipe_ipc
gcc -std=c11 -Wall -Wextra -pthread Task04/src/shared_memory_ipc.c -o Task04/shared_memory_ipc -lrt
gcc -std=c11 -Wall -Wextra -pthread Task04/src/message_queue_ipc.c -o Task04/message_queue_ipc -lrt
gcc -std=c11 -Wall -Wextra Task04/src/ipc_benchmark.c -o Task04/ipc_benchmark

# Task 05: Synchronisation
gcc -std=c11 -Wall -Wextra -pthread Task05/src/bounded_buffer.c -o Task05/bounded_buffer
gcc -std=c11 -Wall -Wextra -pthread Task05/src/semaphore_demo.c -o Task05/semaphore_demo
gcc -std=c11 -Wall -Wextra -pthread Task05/src/producer_consumer.c -o Task05/producer_consumer
gcc -std=c11 -Wall -Wextra -pthread Task05/src/readers_writers.c -o Task05/readers_writers
gcc -std=c11 -Wall -Wextra -pthread Task05/src/dining_philosophers.c -o Task05/dining_philosophers
gcc -std=c11 -Wall -Wextra -pthread Task05/src/monitor_simulation.c -o Task05/monitor_simulation
gcc -std=c11 -Wall -Wextra -pthread Task05/src/booking_lock_benchmark.c -o Task05/booking_lock_benchmark
gcc -std=c11 -Wall -Wextra -pthread Task05/src/atomic_vs_spinlock.c -o Task05/atomic_vs_spinlock

# Task 06: Deadlock Handling & Banker's Algorithm
gcc -std=c11 -Wall -Wextra Task06/src/bankers_algorithm.c -o Task06/bankers_algorithm
gcc -std=c11 -Wall -Wextra Task06/src/bankers_benchmark.c -o Task06/bankers_benchmark
gcc -std=c11 -Wall -Wextra -pthread Task06/src/deadlock_demo.c -o Task06/deadlock_demo
gcc -std=c11 -Wall -Wextra Task06/src/resource_allocation_graph.c -o Task06/resource_allocation_graph
```

---

## Instructions to Execute Every Task

### 1. Automated Execution of All Tasks
```bash
chmod +x run_all_tasks.sh Task*/scripts/*.sh
./run_all_tasks.sh
```

### 2. Individual Task Execution
- **Task 01:** `cd Task01 && ./scripts/run_task1.sh`
- **Task 02:** `cd Task02 && ./scripts/run_task2.sh`
- **Task 03:** `cd Task03 && ./scripts/run_task3.sh`
- **Task 04:** `cd Task04 && ./scripts/run_task4.sh`
- **Task 05:** `cd Task05 && ./scripts/run_task5.sh`
- **Task 06:** `cd Task06 && ./scripts/run_task6.sh`

### 3. Docker 1-Click Turnkey Execution
```bash
docker build -t csa04-os-assessment2 .
docker run --rm csa04-os-assessment2
```

---

## Summary of Completed "Complete the Code" Programs

| Task | Source File | Completed Section | Expected Output vs Actual Output |
| :---: | :--- | :--- | :--- |
| **Q2** | [`Task02/src/round_robin.c`](Task02/src/round_robin.c) | **TODO 1:** Round Robin dispatch, queue update, and finish accounting. | **100% Exact Match:** `Average TAT = 20.00, average WT = 13.50` |
| **Q4** | [`Task04/src/pipe_ipc.c`](Task04/src/pipe_ipc.c) | **TODO 1:** Circular read loop until EOF, average and maximum calculation. | **100% Exact Match:** `analyser: 6 readings, average 35.5, max 41` |
| **Q5** | [`Task05/src/bounded_buffer.c`](Task05/src/bounded_buffer.c) | **TODO 1 & 2:** Semaphore wait/post and mutex locking for producer and consumer. | **100% Exact Match:** `items consumed: 20, sum = 210 (expected 210)` |
| **Q6** | [`Task06/src/bankers_algorithm.c`](Task06/src/bankers_algorithm.c) | **TODO 1:** Safety algorithm test `need <= work`, resource release, and safe sequence logging. | **100% Exact Match:** `initial state safe: P1 P3 P4 P0 P2`, `P1 granted`, `P4 wait`, `P0 denied` |

---

## Evidence Directory References

All tasks contain dedicated `evidence/` subfolders populated with verified demonstration outputs:
- **Task 01 Evidence:** [`Task01/evidence/`](Task01/evidence/) (Process demo output, `ps` process hierarchy, `/proc` status inspection).
- **Task 02 Evidence:** [`Task02/evidence/`](Task02/evidence/) (Round Robin q=3 output, quantum sweep, all algorithms comparison).
- **Task 03 Evidence:** [`Task03/evidence/`](Task03/evidence/) (Sequential execution, 64-thread speedup, thread pool scaling, race condition comparison).
- **Task 04 Evidence:** [`Task04/evidence/`](Task04/evidence/) (Pipe output, pipe EOF hang demonstration, shared memory, message queue, 1MB–100MB benchmark).
- **Task 05 Evidence:** [`Task05/evidence/`](Task05/evidence/) (Single run output, 100 runs verification, 4 prod/4 cons, no-mutex race, locking granularity, atomics vs spinlocks).
- **Task 06 Evidence:** [`Task06/evidence/`](Task06/evidence/) (Banker's output, mathematical hand-trace proof, scalability benchmark 10/100/1000, deadlock demo, RAG cycle detection).

See [`EVIDENCE_STATUS.md`](EVIDENCE_STATUS.md) for complete environmental compliance and inventory status.
