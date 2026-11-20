# Task 07 — Process Control System Calls Evaluation

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assignment 1 (CO1)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Scenario:** 500-Bed Smart Hospital Computing Infrastructure  
**Bloom's Level:** K5 – Evaluate  

---

## 1. Executive Summary

Process control system calls govern the lifecycle of execution contexts in an operating system. On the central hospital database server and clinical workstations, command shells, daemon supervisors, and batch job runners rely on `fork()`, `execve()`, `waitpid()`, and `exit()` to launch diagnostics, execute background queries, and ensure operational resilience.

This evaluation analyzes process lifecycle mechanics, examines how operating systems manage terminated and abandoned processes (zombies and orphans), explores why Unix separates process creation from program loading, provides a detailed post-mortem of a subtle stdio file redirection duplication anomaly, and benchmarks process creation latency.

---

## 2. The Core Process Control Primitives

```
 PARENT PROCESS                               CHILD PROCESS
+--------------------+
| Running Shell      |
| PID = 1000         |
+--------------------+
          |
          | fork() System Call
          |---------------------------------------->+--------------------+
          | (Returns Child PID 1001 to Parent)     | Exact Clone        |
          |                                        | PID = 1001         |
          |                                        | (Returns 0 to Child|
          |                                        +--------------------+
          v                                                  |
+--------------------+                                       | execlp("expr",...)
| waitpid(1001,...)  |                                       v
| Parent Suspends    |                             +--------------------+
| Waiting for Status |                             | Memory Replaced    |
+--------------------+                             | Executes expr 6*7  |
          |                                        +--------------------+
          |                                                  |
          |                                                  | exit(0)
          |           Kernel delivers SIGCHLD                v
          |<=======================================+--------------------+
          |                                        | Zombie State       |
          | Reaps Exit Code (42)                   | Retains Exit Code  |
          v                                        +--------------------+
+--------------------+                                       |
| Parent Resumes     |                                       | Kernel deallocates
| WEXITSTATUS = 0    | <-------------------------------------+ PCB and task_struct
+--------------------+
```

### Functional Breakdown
1. **`fork()`:** Creates a near-identical clone of the calling process. The child inherits copies of parent address space (via Copy-On-Write), file descriptor table, signal dispositions, and environment variables. `fork()` returns the child PID to the parent and `0` to the child.
2. **`exec()` family (e.g., `execlp`):** Completely replaces the current process address space with a new executable binary. It loads code/data sections, initializes a new heap and stack, but preserves open file descriptors (unless `FD_CLOEXEC` is set).
3. **`wait()` / `waitpid()`:** Suspends the calling parent process until the specified child changes state (terminates, stops, or resumes). It extracts the child's exit status code and permits the kernel to release the child's `task_struct`.
4. **`exit()` / `_exit()`:** Terminates process execution. `exit()` flushes all C stdio library buffers and invokes registered `atexit()` handlers before trapping to kernel. `_exit()` directly triggers the `sys_exit` system call without userspace cleanup.

---

## 3. Zombie and Orphan Processes: Lifecycle and OS Handling

```
+-------------------------------------------------------------------------------+
| ZOMBIE PROCESS                                                                |
+-------------------------------------------------------------------------------+
| Definition: A process that has completed execution via exit(), but whose      |
|             parent has not yet called waitpid() to read its exit status.      |
| Kernel State: All physical memory (code, stack, heap) is released. Only the   |
|               Process Control Block (PCB / task_struct) remains in the kernel |
|               process table to retain the PID and exit status.                |
| Problem:      Accumulation exhausts available PIDs (kernel PID limit reached).|
| Resolution:   Parent calls waitpid(); or when parent terminates, init reaps it.|
+-------------------------------------------------------------------------------+

+-------------------------------------------------------------------------------+
| ORPHAN PROCESS                                                                |
+-------------------------------------------------------------------------------+
| Definition: A process whose parent terminates while the child is still        |
|             actively executing.                                               |
| Kernel State: The child has no living parent to receive its SIGCHLD signal.   |
| Resolution:   The operating system automatically reparents the orphan to the  |
|               root process (PID 1 / systemd) or a subreaper process           |
|               (PR_SET_CHILD_SUBREAPER), which immediately reaps it on exit.   |
+-------------------------------------------------------------------------------+
```

---

## 4. Why `fork()` Followed by `exec()` is Superior to a Single "CreateProcess" Call

Operating systems such as Windows utilize a monolithic `CreateProcess()` API that accepts up to 10 parameters. In contrast, the Unix separation into `fork()` and `exec()` represents a foundational architectural masterstroke for shell flexibility:

```
                  +-------------------------------------------+
                  |               Parent Shell                |
                  +-------------------------------------------+
                                        | fork()
                                        v
                  +-------------------------------------------+
                  |         Child (Identical Context)         |
                  +-------------------------------------------+
                                        |
               +------------------------+------------------------+
               | Intermediary Adjustments in Child Before exec() |
               +-------------------------------------------------+
               | 1. I/O Redirection (dup2 to files / pipes)       |
               | 2. Environment Variable Alteration (setenv)      |
               | 3. Working Directory Navigation (chdir)          |
               | 4. Privilege Dropping (setuid / setgid)          |
               | 5. Signal Mask & Resource Limits (setrlimit)     |
               +-------------------------------------------------+
                                        |
                                        v execve()
                  +-------------------------------------------+
                  |          Target Binary Execution          |
                  +-------------------------------------------+
```

Because `fork()` establishes the child process before loading the new binary, the shell can configure file descriptors (connecting pipes `|`, redirecting output `>`), alter user IDs, change root directories (`chroot`), and establish resource limits (`setrlimit`) purely in user code without requiring complex, bloated kernel flags.

---

## 5. Detailed Analysis: The Stdio Buffer Duplication Anomaly

### 5.1 The Experiment (From Task 7 Specification)
The assignment requires modifying `task7.c` into `task7_experiment.c`:
1. Remove the second `fflush(stdout)`.
2. Change child termination from `_exit(3)` to `exit(3)`.
3. Redirect output to a file: `./task7_experiment > task7_exp_redirected.txt`.
4. Observe why `"shell: running job 2
"` appears **twice** in the output file!

### 5.2 Step-by-Step Technical Explanation

```
 INTERACTIVE TERMINAL (Line Buffered)        FILE REDIRECTION (Block Buffered)
+------------------------------------+      +------------------------------------+
| printf("shell: running job 2
");  |      | printf("shell: running job 2
");  |
| Stdio detects isatty(fd) == TRUE.  |      | Stdio detects isatty(fd) == FALSE. |
| Trailing '
' immediately flushes  |      | Switches to 4,096-byte BLOCK BUFFER|
| buffer to screen via write().      |      | Message REMAINS IN USERSPACE BUFFER|
+------------------------------------+      +------------------------------------+
                  |                                           |
                  | fork()                                    | fork()
                  v                                           v
+------------------------------------+      +------------------------------------+
| Buffer is empty.                   |      | Parent and Child both possess      |
| Child inherits empty buffer.       |      | "shell: running job 2
" in memory!|
+------------------------------------+      +------------------------------------+
                  |                                           |
                  |                                           v
                  |                         +------------------------------------+
                  |                         | Child executes exit(3)             |
                  |                         | - Standard library exit() FLUSHES  |
                  |                         |   the inherited buffer to file!    |
                  |                         |   (1st instance written)           |
                  |                         +------------------------------------+
                  |                                           |
                  |                                           v
                  |                         +------------------------------------+
                  |                         | Parent resumes and terminates:     |
                  |                         | - Parent's exit() flushes its OWN  |
                  |                         |   buffer to the same file!         |
                  |                         |   (2nd instance written - DUP!)    |
                  +-------------------------+------------------------------------+
```

1. **Buffering Mode Transition:** When standard output is connected to an interactive terminal, glibc configures `stdout` as **line-buffered** (flushing automatically upon encountering `
`). When stdout is redirected to a disk file, glibc automatically switches `stdout` to **block-buffered** (accumulating up to 4,096 bytes before executing a kernel `write()`).
2. **Buffer Inheritance Across `fork()`:** Without `fflush(stdout)`, the string `"shell: running job 2
"` resides in the parent's user-space stdio buffer. When `fork()` is called, the child's address space is cloned via Copy-On-Write, inheriting the identical unflushed buffer!
3. **Difference Between `exit()` and `_exit()`:**
   * `exit(3)` is the C runtime library function. Prior to kernel termination, it flushes all open stdio streams, causing the child to write the string to the file.
   * `_exit(3)` is the direct kernel system call (`sys_exit`). It immediately destroys the process context without flushing userspace stdio buffers.
4. When `exit(3)` is used, the child writes the string once, and when the parent subsequently exits, the parent flushes its copy of the buffer again, producing the duplicate output line.

---

## 6. Alternative Concept: `posix_spawn()` and Threads vs `fork()+exec()`

### 6.1 Algorithmic Evaluation for 1,000 Command Spawns

```
+---------------------------+---------------------------+---------------------------+
| Dimension                 | fork() + execve()         | posix_spawn()             |
+---------------------------+---------------------------+---------------------------+
| Memory Handling           | Clones all page tables    | Uses vfork() / CLONE_VM   |
|                           | via Copy-On-Write (COW)   | (Zero page table copying) |
| Latency per Spawn         | 800 - 1,500 us            | 200 - 450 us              |
| Memory Consumption        | O(P) page table frames    | O(1) shared memory context|
| Isolation & Fault Boundary| Complete hardware MMU     | Complete hardware MMU     |
|                           | memory isolation          | memory isolation          |
+---------------------------+---------------------------+---------------------------+
```

For large parent processes (e.g., an in-memory hospital database allocating $64\,	ext{GB}$ of RAM), calling `fork()` requires cloning millions of page table entries. Even though memory pages are not duplicated immediately, the page table duplication overhead can induce a 50-millisecond scheduling pause. `posix_spawn()` bypasses page table cloning by executing in the parent's address space until `execve()` is called, cutting command launch latency by up to 75%.
