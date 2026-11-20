# Task 05 — System Calls Evaluation & Trace Analysis

**Course Code:** CSA04 — Operating Systems  
**Assignment:** Assignment 1 (CO1)  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Scenario:** 500-Bed Smart Hospital Computing Infrastructure  
**Bloom's Level:** K5 – Evaluate  

---

## 1. Executive Overview

System calls represent the fundamental programmatic interface between user-space application code and the protected kernel space of the operating system. Because direct hardware access by untrusted or buggy user programs could crash the machine, compromise patient data confidentiality, or violate real-time deadlines, modern processor architectures strictly prohibit direct peripheral access from unprivileged CPU modes.

This analysis examines how application programs request operating system services, traces the complete lifecycle of a `read()` system call from hardware trap to kernel dispatch, explores architectural parameter passing conventions, inspects `strace` mechanics, and evaluates the performance and memory tradeoffs between raw POSIX system calls and standard C buffered I/O.

---

## 2. The Six Fundamental Categories of System Calls

Modern POSIX-compliant operating systems categorize system calls according to functional subsystem:

```
+---------------------------------------------------------------------------------+
|                        CATEGORIES OF SYSTEM CALLS                               |
+---------------------------------------------------------------------------------+
| 1. Process Control         | fork(), execve(), waitpid(), exit(), brk()         |
| 2. File Management         | open(), read(), write(), close(), stat(), unlink() |
| 3. Device Management       | ioctl(), tcgetattr(), tcsetattr(), read(), write() |
| 4. Information Maintenance | getpid(), time(), uname(), getrusage(), alarm()    |
| 5. Communications (IPC)    | socket(), bind(), connect(), pipe(), shmget()      |
| 6. Protection & Security   | chmod(), chown(), umask(), setuid(), capset()      |
+---------------------------------------------------------------------------------+
```

### Demonstration in Smart Hospital Workloads
* **Process Control:** The database server forks worker child processes or worker threads to service incoming telemetry streams from ward monitors.
* **File Management:** Imaging servers write high-resolution MRI scans (DICOM format) to persistent NVMe arrays via `open()`, `write()`, and `close()`.
* **Device Management:** Bedside monitor drivers configure digital signal acquisition rates on serial/USB electrocardiogram (ECG) interfaces via `ioctl()`.
* **Information Maintenance:** Clinical audit loggers invoke `time()` and `getpid()` to timestamp every prescription update.
* **Communications:** Nurse tablets stream medication barcode validation requests to the central server over TCP `socket()` connections.
* **Protection:** The hospital operating system invokes `umask(0027)` and `chmod()` to ensure patient records are inaccessible to unauthorized local users.

---

## 3. Step-by-Step Lifecycle of a `read()` System Call

When a clinical application requests 4,096 bytes from a patient telemetry log via `read(fd, buffer, 4096)`, the processor transitions through a rigorously sequenced 5-stage transition:

```
 USER SPACE (Ring 3)                                    KERNEL SPACE (Ring 0)
+------------------------+                             +------------------------+
| 1. Application calls   |                             |                        |
|    read(fd, buf, 4096) |                             |                        |
+------------------------+                             |                        |
            |                                          |                        |
            v                                          |                        |
+------------------------+                             |                        |
| 2. C Library Wrapper   |                             |                        |
|    - Load %rax = 0     |                             |                        |
|    - Load parameters   |                             |                        |
|      into registers    |                             |                        |
|    - Execute SYSCALL   | --------------------------> | 3. Hardware Trap       |
+------------------------+      Privilege Switch       |    - Switch to Ring 0  |
                                (Ring 3 -> Ring 0)     |    - Save user %rsp,%rip|
                                                       |    - Jump to MSR_LSTAR |
                                                       +------------------------+
                                                                    |
                                                                    v
                                                       +------------------------+
                                                       | 4. Kernel Dispatch     |
                                                       |    - Save registers    |
                                                       |    - Call sys_call_    |
                                                       |      table[0]          |
                                                       |    - VFS -> File Driver|
                                                       |    - Copy data to buf  |
                                                       +------------------------+
                                                                    |
            +-------------------------------------------------------+
            |                  Privilege Switch
            v                  (Ring 0 -> Ring 3)
+------------------------+
| 5. C Library Returns   |
|    - Check %rax        |
|    - Return bytes read |
+------------------------+
```

### Detailed Execution Sequence

1. **Step 1: User-Space Library Call:**  
   The application invokes the POSIX C library function `read()`. The glibc wrapper places the architectural arguments into designated hardware registers according to the System V AMD64 ABI:
   * System call number for `read` (`0` on x86-64) $	o$ `%rax`
   * File descriptor `fd` $	o$ `%rdi` (1st parameter)
   * User buffer pointer `buf` $	o$ `%rsi` (2nd parameter)
   * Byte count `4096` $	o$ `%rdx` (3rd parameter)

2. **Step 2: Hardware Trap Instruction:**  
   The CPU executes the `syscall` machine instruction. This causes the CPU hardware to:
   * Save the current program counter `%rip` into register `%rcx`.
   * Save the current CPU flags register `%rflags` into register `%r11`.
   * Clear the privilege mode bits in the code segment register (`CS`), immediately transitioning the CPU execution state from **User Mode (Ring 3)** to **Kernel Mode (Ring 0)**.
   * Load the new `%rip` from the Model Specific Register `MSR_LSTAR` (which points to the kernel's low-level system call entry point `entry_SYSCALL_64`).

3. **Step 3: Kernel Context Save & System Call Dispatch:**  
   Operating in Ring 0, the kernel:
   * Swaps to the designated per-thread kernel stack (`SWAPGS` instruction).
   * Pushes the user register state onto the kernel stack (`struct pt_regs`).
   * Validates that the system call number in `%rax` does not exceed `NR_syscalls`.
   * Indexes into the kernel system call table: `call *sys_call_table(, %rax, 8)`, invoking `sys_read()`.

4. **Step 4: Kernel Work & Data Transfer:**  
   Inside `sys_read()`:
   * The Virtual File System (VFS) layer validates the file descriptor `fd` against the process file descriptor table `current->files`.
   * Resolves the underlying `struct file` and verifies read permissions.
   * Invokes the filesystem-specific read method (e.g., ext4, NFS, or character device driver).
   * Checks the Linux page cache. If the page is missing, initiates a Direct Memory Access (DMA) transfer from the NVMe storage drive into kernel physical memory.
   * Copies the requested bytes from kernel page cache into user-space memory `buf` using `copy_to_user()`, safely catching invalid memory addresses without causing a kernel panic.

5. **Step 5: Return to User Space:**  
   The kernel writes the number of bytes transferred into `%rax` (or negative error code such as `-EFAULT`). The kernel restores saved user registers, executes `sysretq`, which restores Ring 3 privilege and reloads user `%rip` from `%rcx`. Control returns to the glibc wrapper, which sets `errno` if `%rax < 0` and returns the final byte count to the calling application.

---

## 4. Parameter Passing Mechanisms in Operating Systems

Operating systems employ three architectural techniques to pass arguments from user applications to the kernel:

```
+-----------------------------------------------------------------------------------+
| METHOD 1: CPU REGISTERS                                                           |
| Fast (<1 ns). Limited by physical register count.                                 |
| x86-64 System V ABI: %rax (syscall #), %rdi (arg1), %rsi (arg2), %rdx (arg3),      |
|                      %r10 (arg4), %r8 (arg5), %r9 (arg6).                         |
+-----------------------------------------------------------------------------------+
                                         |
                                         v
+-----------------------------------------------------------------------------------+
| METHOD 2: MEMORY BLOCK / STRUCT POINTER                                           |
| For complex or unbounded data structures (e.g., struct stat, struct sockaddr).    |
| Application populates struct in user RAM; passes memory pointer in register %rdi.  |
| Kernel verifies pointer validity and reads contents using copy_from_user().       |
+-----------------------------------------------------------------------------------+
                                         |
                                         v
+-----------------------------------------------------------------------------------+
| METHOD 3: PROGRAM STACK                                                           |
| Historical convention (x86-32 int 0x80 pushed args to stack). Slow due to memory |
| bus references; superseded by register-based SYSCALL/SYSENTER instructions.      |
+-----------------------------------------------------------------------------------+
```

---

## 5. How `strace` Observes System Calls

The Linux diagnostic utility `strace` intercepts and records all system calls made by a process using the kernel `ptrace()` debugging facility:

```
+----------------+                       +----------------+
| strace Process |                       | Target Program |
+----------------+                       +----------------+
       |                                         |
       |  ptrace(PTRACE_SEIZE/SYSCALL)           |
       |---------------------------------------->|
       |                                         |
       |                                         | Executes read()
       |                                         |        |
       |  Target stops at syscall-enter-stop     |        v
       |<----------------------------------------|  CPU traps into kernel
       |                                         |
       |  strace inspects %rax, %rdi, %rsi       |
       |  Decodes syscall name & arguments       |
       |                                         |
       |  ptrace(PTRACE_SYSCALL, resume)         |
       |---------------------------------------->|
       |                                         | Kernel executes read()
       |                                         |        |
       |  Target stops at syscall-exit-stop      |        v
       |<----------------------------------------| Kernel finishes syscall
       |                                         |
       |  strace inspects return code in %rax    |
       |  Prints decoded line to stderr          |
       |                                         |
       v                                         v
```

By intercepting both entry and exit, `strace` captures exact arguments, returned byte counts, execution duration, and error codes (`ENOENT`, `EACCES`) without modifying application source code.

---

## 6. Alternative Concept and Complexity Analysis

### 6.1 Problem Formulation: Standard I/O Library (`fread`/`fwrite`) vs Raw System Calls (`read`/`write`)

An alternative concept replaces direct POSIX system calls (`open`, `read`, `write`) with C standard I/O library streaming functions (`fopen`, `fread`, `fwrite`). We evaluate the mathematical system call frequency and memory overhead for a file of $n$ bytes copied using a buffer of $b$ bytes.

### 6.2 Mathematical Model & Complexity Bounds

Let $n$ be total file size in bytes, and $b$ be application request size in bytes:

$$	ext{System Calls (Raw POSIX)} = \left\lceil rac{n}{b} ightceil + 1 \quad (	ext{including terminating EOF read})$$

$$	ext{System Calls (Stdio Library)} = \left\lceil rac{n}{B_{stream}} ightceil + 1 \quad (	ext{where } B_{stream} = 4096 	ext{ or } 8192\,	ext{bytes})$$

```
+---------------------------------------+---------------------------------------+
| Metric                                | Raw System Calls (read/write)         | Stdio Library (fread/fwrite)          |
+---------------------------------------+---------------------------------------+
| Time Complexity (Kernel Traps)        | O(n / b) traps                        | O(n / B_stream) traps                 |
| For b = 1 byte (Single-byte reads)    | 10,001 system calls                   | 4 system calls                        |
| For b = 4,096 bytes                   | 4 system calls                        | 4 system calls                        |
| Space Complexity (Userspace Memory)   | O(b) buffer space                     | O(b + B_stream) (4KB stream buffer)   |
| Context Switching Overhead            | Proportional to n/b                   | Bounded to n / 4096                   |
+---------------------------------------+---------------------------------------+
```

### 6.3 Analytical Findings
1. When applications read character-by-character ($b = 1$ byte) across a 10,000-byte patient record, raw POSIX calls execute **10,001 hardware traps**, causing extreme CPU pipeline stalling and mode switch overhead.
2. In contrast, `fread()` buffers data in an internal 4,096-byte userspace buffer, servicing 4,095 requests entirely in user mode and executing only **4 system calls** ($99.96\%$ reduction in system calls).
3. When $b \ge 4096$, raw system calls match stdio performance without incurring the secondary `memcpy()` and `FILE` struct memory overhead.
