# Task A1.2 – Process and Memory Management in Linux (3 Marks)

**Aim:** evaluate the Completely Fair Scheduler (CFS), the page cache, swapping, and the `/proc` and `/sys` file systems as tools for **observing and managing the result portal server**.

## 1. Completely Fair Scheduler (CFS)

### 1.1 How it works
CFS aims to give every runnable task a fair share of CPU time, as if the CPU could be divided equally among them.
- Each task has a **virtual runtime (`vruntime`)**: the CPU time it has consumed, scaled by its weight.
- Runnable tasks are kept in a **red-black tree** ordered by `vruntime`. The task with the smallest value (leftmost node) has received the least service and **runs next** ([Diagram D3](diagrams/cfs_rbtree.md)).
- When a task runs, its `vruntime` grows. A lower-priority task (higher `nice` value, smaller weight) has its `vruntime` grow faster, so it receives less CPU. A sleeping task keeps a low `vruntime` and is quickly scheduled when it wakes, which favours interactive and I/O-bound tasks.
- There are no fixed time slices; the slice is derived from a target latency divided among runnable tasks, bounded by a minimum granularity.
- Cost: picking the next task is O(1) with the leftmost node cached; inserting or removing a task is **O(log n)** for n runnable tasks. Each CPU has its own run queue, with periodic load balancing between CPUs.

*Version note:* from Linux 6.6 the kernel's fair-class scheduler was changed from CFS to **EEVDF** (Earliest Eligible Virtual Deadline First), which keeps the fairness idea but adds explicit latency handling. The assignment asks for CFS, so CFS is evaluated here; the administrative observations below apply to both. The student should check the kernel version (`uname -r`) of the server actually used.

### 1.2 Evaluation for the portal

| Aspect | Assessment |
|---|---|
| Fairness | Good: many Gunicorn workers and Nginx workers share CPUs without starvation. |
| Interactivity | Good: tasks that mostly wait for network or disk wake with low `vruntime`, keeping request latency low. |
| Control | `nice`/`renice`, CPU affinity and **cgroup CPU weights/quotas** (used by systemd, `CPUWeight=`) let the administrator favour PostgreSQL or Nginx over background backups. |
| Weakness | Fairness is not a guarantee of deadlines; under overload every task slows equally, so the server must be sized and protected by limits. CFS is not a real-time scheduler. |
| Weakness | Too many runnable workers cause context-switch overhead. Worker count should be matched to cores (a common starting rule is 2 × cores + 1 for Gunicorn, to be tuned by measurement). |

**Management actions:** run backups with `nice -n 19` and `ionice -c3` so they do not compete with student requests; give the database service a higher `CPUWeight`.

## 2. Page Cache

- The page cache keeps recently read file data, and data waiting to be written, in otherwise unused RAM. Reads that hit the cache avoid disk access; writes are placed in cache as *dirty pages* and written back later by kernel flusher threads (or forced by `fsync`).
- The kernel also performs **read-ahead**, fetching following blocks when it detects sequential reads.
- The cache is index-searched by (file, page offset), which takes roughly O(1)–O(log n) per lookup; it is far faster than disk access, whose latency is many orders of magnitude higher than RAM.
- Cache memory is **reclaimable**: when programs need RAM, clean cache pages are simply dropped. Therefore low "free" memory is not a problem by itself; "available" memory (`free -h`) is the correct measure.

*Evaluation for the portal:* result PDFs, static files, Python code and database files are read repeatedly by many students, so a large page cache greatly reduces disk I/O. Risks: dirty-page write-back can cause short I/O stalls under heavy writes, and losing power before write-back loses unsynced data, which is why the database uses its own write-ahead log with `fsync`.

## 3. Swapping

- When RAM is short, the kernel moves rarely used **anonymous pages** (heap, stack, which have no file backing) to **swap space** on disk and frees the frames. Clean file-backed pages need no swap because they can be re-read from their file.
- The tendency to swap versus drop cache is influenced by `vm.swappiness`.
- Swap is a safety margin, not extra performance. Disk is much slower than RAM, so a system that constantly swaps in and out suffers **thrashing**: the CPU waits for pages and response time collapses.
- If both RAM and swap are exhausted, the **OOM killer** ends a process, which could be the database.

*Evaluation for the portal:* a small amount of swap is useful to absorb short spikes and to move idle pages out. On result day the aim is **no sustained swapping**; the signal is non-zero `si`/`so` columns in `vmstat` over many seconds. Remedies are limiting Gunicorn workers, setting memory limits per service (`MemoryMax=`), adding RAM, or lowering `vm.swappiness` for the database host.

## 4. The `/proc` File System

`/proc` is a **virtual file system**: its files occupy no disk space and are generated by the kernel when read. It exposes live process and kernel information.

| Path | Information | Use on the portal server |
|---|---|---|
| `/proc/loadavg` | Run-queue load averages | Quick overload check (compare with number of CPUs) |
| `/proc/meminfo` | `MemTotal`, `MemAvailable`, `Cached`, `Dirty`, `SwapFree` | Source of `free`; watch memory and swap |
| `/proc/cpuinfo` | CPU model, cores | Capacity planning |
| `/proc/stat` | CPU time counters | Source for `top` and `vmstat` |
| `/proc/<pid>/status` | State, memory, threads for a process | Check a Gunicorn worker's memory |
| `/proc/<pid>/fd/` | Open file descriptors | Detect descriptor leaks (many open sockets) |
| `/proc/<pid>/cmdline`, `/proc/<pid>/limits` | Command and resource limits | Verify service limits |
| `/proc/net/tcp`, `/proc/net/dev` | Sockets and interface counters | Source for `ss` and network statistics |
| `/proc/sys/` | Tunable kernel parameters (`sysctl`) | Tune `net.core.somaxconn`, `vm.swappiness` |

Common tools such as `ps`, `top`, `free` and `vmstat` are mainly readers of `/proc`. Because it is read at runtime with no agent, `/proc` suits lightweight monitoring scripts.

## 5. The `/sys` File System

`/sys` (sysfs) is another virtual file system that exports the kernel's **device and driver model** as a hierarchy of directories and attribute files. Unlike `/proc`, it is well structured and many attributes are writable, so it is used to *control* devices.

| Path | Information | Use |
|---|---|---|
| `/sys/class/net/<if>/` | Interface state, speed, MTU, statistics | Check NIC link speed and errors |
| `/sys/block/<dev>/queue/scheduler` | Disk I/O scheduler | View or change the I/O scheduler |
| `/sys/block/<dev>/queue/rotational` | HDD (1) or SSD (0) | Decide tuning |
| `/sys/devices/system/cpu/` | CPU online state, frequency governor | Confirm CPUs and power policy |
| `/sys/fs/cgroup/` | Control groups (resource limits and usage) | Verify per-service CPU and memory limits |
| `/sys/kernel/mm/transparent_hugepage/` | Memory-page settings | Database tuning |

## 6. Importance for Observing and Managing the Portal Server

| Need on result day | Mechanism | How it helps |
|---|---|---|
| Is the CPU overloaded? | `/proc/loadavg`, `/proc/stat` (`top`, `vmstat`) | Detect run-queue growth caused by too many runnable tasks |
| Is memory exhausted? | `/proc/meminfo` (`free`) | Check available memory, dirty pages |
| Is the server swapping? | `vmstat` `si`/`so` | Detect thrashing early |
| Is the disk the bottleneck? | `iostat`, `/sys/block/*/stat` | Distinguish I/O wait from CPU load |
| Which process misbehaves? | `/proc/<pid>/status`, `ps`, `top` | Find a leaking or runaway worker |
| Are limits enforced? | `/sys/fs/cgroup` | Confirm `MemoryMax=` and `CPUQuota=` on services |
| Is the network saturated? | `/sys/class/net`, `/proc/net/dev`, `ss` | Check errors and connection counts |

Because these interfaces are plain files, the same information can be read by shell scripts and feed the alerting in [A2_Automation_Plan.md](A2_Automation_Plan.md). No measured values are reported here: real readings belong to the Part B implementation.

## 7. Conclusion

CFS gives the portal fair, low-latency CPU sharing at O(log n) cost per scheduling event, but it needs worker counts and cgroup limits chosen sensibly. The page cache is the main reason a read-heavy results portal can serve many students from RAM, while swapping must be treated as an emergency buffer rather than capacity. `/proc` and `/sys` give the administrator, with no extra software, the facts needed to detect overload early and to enforce resource policy.
