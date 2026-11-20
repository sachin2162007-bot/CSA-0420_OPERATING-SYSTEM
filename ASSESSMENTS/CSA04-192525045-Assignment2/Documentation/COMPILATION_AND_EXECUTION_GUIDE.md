# Compilation and Execution Guide

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assessment 2 (CO2)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  

---

## 1. Prerequisites for Linux Environment
To compile and execute all programs on an Ubuntu/Debian system:
```bash
sudo apt-get update
sudo apt-get install -y build-essential gcc make time bash coreutils procps
```

---

## 2. One-Click Master Execution
From the root of the repository:
```bash
chmod +x run_all_tasks.sh Task*/scripts/*.sh
./run_all_tasks.sh
```

---

## 3. Individual Task-by-Task Compilation & Running

### Task 01: Processes & Process States
```bash
cd Task01
gcc -std=c11 -Wall -Wextra -pedantic src/process_demo.c -o process_demo
./process_demo 3
./scripts/run_task1.sh
cd ..
```

### Task 02: CPU Scheduling
```bash
cd Task02
gcc -std=c11 -Wall -Wextra src/round_robin.c -o round_robin
./round_robin 3
./round_robin 1
./round_robin 5
./round_robin 20
gcc -std=c11 -Wall -Wextra src/scheduling_comparison.c -o scheduling_comparison
./scheduling_comparison
./scripts/run_task2.sh
cd ..
```

### Task 03: Threads
```bash
cd Task03
gcc -std=c11 -Wall -Wextra src/camera_sequential.c -o camera_sequential
gcc -std=c11 -Wall -Wextra -pthread src/camera_threads.c -o camera_threads
gcc -std=c11 -Wall -Wextra -pthread src/camera_thread_pool.c -o camera_thread_pool
gcc -std=c11 -Wall -Wextra -pthread src/race_condition_demo.c -o race_condition_demo
./camera_sequential
./camera_threads
./camera_thread_pool
./race_condition_demo
./scripts/run_task3.sh
cd ..
```

### Task 04: Inter-Process Communication
```bash
cd Task04
gcc -std=c11 -Wall -Wextra src/pipe_ipc.c -o pipe_ipc
./pipe_ipc
gcc -std=c11 -Wall -Wextra -pthread src/shared_memory_ipc.c -o shared_memory_ipc -lrt
./shared_memory_ipc
gcc -std=c11 -Wall -Wextra -pthread src/message_queue_ipc.c -o message_queue_ipc -lrt
./message_queue_ipc
gcc -std=c11 -Wall -Wextra src/ipc_benchmark.c -o ipc_benchmark
./ipc_benchmark
./scripts/run_task4.sh
cd ..
```

### Task 05: Synchronisation
```bash
cd Task05
gcc -std=c11 -Wall -Wextra -pthread src/bounded_buffer.c -o bounded_buffer
./bounded_buffer
./bounded_buffer -multi
gcc -std=c11 -Wall -Wextra -pthread src/booking_lock_benchmark.c -o booking_lock_benchmark
./booking_lock_benchmark
gcc -std=c11 -Wall -Wextra -pthread src/atomic_vs_spinlock.c -o atomic_vs_spinlock
./atomic_vs_spinlock
./scripts/run_task5.sh
cd ..
```

### Task 06: Deadlock Handling & Banker's Algorithm
```bash
cd Task06
gcc -std=c11 -Wall -Wextra src/bankers_algorithm.c -o bankers_algorithm
./bankers_algorithm
gcc -std=c11 -Wall -Wextra src/bankers_benchmark.c -o bankers_benchmark
./bankers_benchmark
gcc -std=c11 -Wall -Wextra src/resource_allocation_graph.c -o resource_allocation_graph
./resource_allocation_graph
./scripts/run_task6.sh
cd ..
```

---

## 4. Docker Turnkey Execution
To run in an isolated Ubuntu container without installing local packages:
```bash
docker build -t csa04-assessment2 .
docker run --rm csa04-assessment2
```
