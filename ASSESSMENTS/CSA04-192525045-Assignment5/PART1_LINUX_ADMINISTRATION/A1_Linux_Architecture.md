# Task A1.1 – Linux Architecture (2 Marks)

**Course:** CSA04 – Operating Systems | **Assignment 5 (CO5)** | **Bloom's level:** K5 – Evaluate
**Context:** University Result Portal (about 150,000 students logging in within two hours on result day)

## 1. Overview

Linux is a multi-user, multitasking, Unix-like operating system. Its architecture is layered: hardware at the bottom, the **kernel** above it, **system libraries** above the kernel interface, and the **shell, utilities and applications** at the top (see [Diagram D1](diagrams/linux_architecture.md)). Applications never touch hardware directly. They request services through **system calls**, and the kernel, running in privileged mode, performs the work on their behalf. This separation protects the system: a failing portal process cannot overwrite kernel memory or another user's data.

## 2. Linux Kernel

The kernel is the core program that runs in kernel space with full hardware access. Linux uses a **monolithic, modular** design: the major services run inside one kernel image for speed, while drivers and file systems can be loaded as modules (`lsmod`, `modprobe`) without rebooting. Its responsibilities are to share the CPU, memory, storage and network among processes, to enforce protection between users, and to give every program a uniform interface (system calls).

*Evaluation for the portal:* the monolithic design gives low overhead for the many system calls a busy web server makes. Its drawback is that a faulty driver can crash the whole system, so only trusted, well-tested drivers should be used on the result-day server.

## 3. System Libraries

System libraries are collections of code that wrap system calls in convenient functions. The main one is the **GNU C library (glibc)**, which provides `open()`, `read()`, `malloc()`, `fork()` and socket functions; other examples are `libpthread` (threads) and `libssl` (TLS). A library function such as `printf()` or `fopen()` does the formatting and buffering in user space and calls the kernel only when needed, which reduces the number of expensive user-to-kernel transitions. Libraries can be shared, so one copy in RAM serves many programs. Python, Gunicorn, Nginx and PostgreSQL all depend on glibc.

## 4. Shell and Utilities

The **shell** (for example `bash`) is the command interpreter. It reads commands, starts programs (using `fork` and `exec`), connects them with pipes and redirection, and runs scripts with variables and control flow. **Utilities** are small programs that each do one job: `ls`, `cp`, `grep`, `awk`, `ps`, `top`, `df`, `ss`, `systemctl`, `journalctl`. Combined with pipes (`grep ERROR app.log | sort | uniq -c`) they let an administrator observe and automate the portal server without writing a full program. This is the basis of the automation plan in [A2_Automation_Plan.md](A2_Automation_Plan.md).

## 5. How the Kernel Manages Resources

[Diagram D2](diagrams/kernel_subsystems.md) shows one portal request passing through these subsystems.

### 5.1 Process management
- Each process or thread is described by a kernel structure (`task_struct`) holding its state, priority, memory map, open files and credentials.
- New processes are created with `fork()`/`clone()` and given a new program with `exec()`. Gunicorn uses this to start several worker processes.
- Process states include running, ready, sleeping (waiting for I/O) and zombie (finished but not yet reaped by the parent).
- The **scheduler** decides which ready task runs on which CPU (see [A1_Process_Memory_Management.md](A1_Process_Memory_Management.md)).
- Signals (`SIGTERM`, `SIGKILL`) and inter-process communication (pipes, sockets, shared memory) allow control and cooperation.

### 5.2 Memory management
- Every process receives its own **virtual address space**. The MMU and page tables translate virtual addresses to physical frames, so processes are isolated from one another.
- Memory is managed in fixed-size **pages** (commonly 4 KiB). Pages are allocated on demand (demand paging) and shared when possible (shared libraries, copy-on-write after `fork`).
- Free RAM is used as the **page cache** for file data, and rarely used pages can be moved to **swap**.
- If memory is exhausted and cannot be reclaimed, the **OOM killer** terminates a process to save the system.

### 5.3 File management
- The **Virtual File System (VFS)** presents one directory tree and one set of calls (`open`, `read`, `write`, `close`) over many file-system types (ext4, XFS, tmpfs, NFS).
- Files are described by **inodes** (owner, permissions, size, block locations), and names are mapped to inodes by directories.
- The kernel enforces permissions on every access and buffers file data in the page cache. Details are in [A1_Filesystem_Permissions.md](A1_Filesystem_Permissions.md).

### 5.4 Device management
- Hardware is accessed through **device drivers**. Devices appear as files in `/dev` (block devices such as `sda`, character devices such as `tty`).
- Devices signal the CPU through **interrupts**; the kernel handles them and passes data to waiting processes. Drivers may also use DMA to move data without CPU involvement.
- `udev` creates device nodes dynamically, and the block layer orders disk requests with an I/O scheduler to improve throughput.
- Device information is exported through `/sys` (see A1.2).

### 5.5 Network management
- The kernel implements the protocol stack (Ethernet, IP, TCP, UDP) and the **socket** interface used by Nginx, Gunicorn and PostgreSQL.
- Incoming packets are received by the NIC driver, processed by the IP and TCP layers, and delivered to the process that owns the destination socket. Outgoing data follows the reverse path.
- **netfilter** (used by `iptables`/`nftables` and `ufw`) filters and translates packets in the kernel, providing the firewall described in [A2_Network_Architecture.md](A2_Network_Architecture.md).
- TCP connection handling (accept queue, backlog, congestion control) strongly affects how many simultaneous students the portal can serve.

## 6. Summary Table

| Component | Role | Portal example |
|---|---|---|
| Kernel | Controls hardware, enforces protection, offers system calls | Schedules Nginx/Gunicorn, manages RAM and TCP |
| System libraries | Wrap system calls, give standard functions | glibc used by Python and PostgreSQL |
| Shell | Interprets commands and scripts | `bash` runs backup and alert scripts |
| Utilities | Small tools for observation and control | `top`, `df`, `ss`, `journalctl` |
| Process management | Creates and schedules tasks | Gunicorn workers |
| Memory management | Virtual memory, page cache, swap | Cached result PDFs, database buffers |
| File management | VFS, inodes, permissions | `/srv/portal`, `/var/log/nginx` |
| Device management | Drivers, interrupts, `/dev` | Disk and NIC drivers |
| Network management | Sockets, TCP/IP, netfilter | HTTPS connections, firewall rules |
