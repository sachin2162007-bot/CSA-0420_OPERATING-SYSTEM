# B1.1 – Observe the System (2 marks)

**All output below is ACTUAL output** captured on 21 Nov 2026 in the sandbox described in the README (Ubuntu 24.04.5, kernel 6.18.44, **1 vCPU, 3992 MiB RAM, no swap**, a Firecracker micro-VM). It is not a physical university server, so values are small and mostly idle. Two captures are shown: a **baseline** (idle) and a **load** capture taken while `ab` sent 6,000 requests (20 concurrent) through Nginx to Gunicorn. The `ab` client ran on the same single CPU, so the load numbers are pessimistic for the server.

## 1. Baseline (idle) capture

```text
$ date; uname -srmo; cat /etc/os-release | head -2; nproc
Sat Nov 21 06:29:45 UTC 2026
Linux 6.18.44-fc-v726 x86_64 GNU/Linux
PRETTY_NAME="Ubuntu 24.04.5 LTS"
NAME="Ubuntu"
1

$ ps aux --sort=-%mem | head -8
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root      1824  0.1  1.9 1833912 80968 ?       Ssl  06:28   0:00 dockerd
root      1831  0.1  1.1 1792784 46868 ?       Ssl  06:28   0:00 containerd --config /var/run/docker/containerd/containerd.toml
root        66  0.0  0.7 1955820 30872 ?       Sl   06:27   0:00 /opt/rclone/rclone-filestore multimount --config /dev/shm/rclone-boot/config.json
root         1  0.2  0.1  32756  5648 ?        SLl  06:27   0:00 /process_api --firecracker-init --addr 0.0.0.0:2024 --max-ws-buffer-size 32768 --block-local-connections --log-vsock-port 5002 --control-vsock-port 5001 --listen-vsock-port 2024 --tcp-readiness-only
root      2095  0.0  0.1   7908  4176 ?        R    06:29   0:00 ps aux --sort=-%mem
64321       65  0.0  0.0   7336  3024 ?        SL   06:27   0:00 sbx-telemetry-collector
root      2073  0.0  0.0   2812  2024 ?        S    06:29   0:00 /bin/sh -c cd /home/claude || cd /tmp || exit; unset OLDPWD; mkdir -p /home/claude/ev; cd /home/claude/PART3_LINUX_DOCKER_IMPLEMENTATION/Task01 { for t in "" 1 abc; do echo "\$ ./disk_space_alert.sh $t"; ./disk_space_alert.sh $t 2>&1; echo "exit status: $?"; done; } > /home/claude/ev/disk_alert.txt 2>&1; cat /home/claude/ev/disk_alert.txt # observation (run while a little load is present) O=/home/claude/ev/observe.txt; : > $O run(){ echo "\$ $*" >> $O; eval "$*" >> $O 2>&1; echo >> $O; } run "date; uname -srmo; cat /etc/os-release | head -2; nproc" run "ps aux --sort=-%mem | head -8" run "ps -eo pid,ppid,user,stat,ni,%cpu,%mem,rss,comm --sort=-rss | head -6" run "ps -e --no-headers | wc -l" run "top -b -n 1 | head -15" run "free -m" run "vmstat 1 5" run "iostat -x 1 2" run "df -h" run "df -i / | head -3" run "ss -tulpn" run "ss -s" cat $O | head -150

$ ps -eo pid,ppid,user,stat,ni,%cpu,%mem,rss,comm --sort=-rss | head -6
  PID  PPID USER     STAT  NI %CPU %MEM   RSS COMMAND
 1824     1 root     Ssl    0  0.1  1.9 80968 dockerd
 1831  1824 root     Ssl    0  0.1  1.1 46868 containerd
   66     1 root     Sl     0  0.0  0.7 30872 rclone-filestor
    1     0 root     SLl    0  0.2  0.1  5648 process_api
 2097  2073 root     R      0  0.0  0.1  4160 ps

$ ps -e --no-headers | wc -l
55

$ top -b -n 1 | head -15
top - 06:29:45 up 1 min,  0 user,  load average: 0.04, 0.02, 0.00
Tasks:  55 total,   1 running,  54 sleeping,   0 stopped,   0 zombie
%Cpu(s):  0.0 us,  0.0 sy,  0.0 ni,100.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st 
MiB Mem :   3992.3 total,   3262.8 free,    321.4 used,    633.1 buff/cache     
MiB Swap:      0.0 total,      0.0 free,      0.0 used.   3670.9 avail Mem 

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
    1 root      20   0   32756   5648   5116 S   0.0   0.1   0:00.31 process_a+
    2 root      20   0       0      0      0 S   0.0   0.0   0:00.00 kthreadd
    3 root      20   0       0      0      0 S   0.0   0.0   0:00.00 pool_work+
    4 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R+
    5 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R+
    6 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R+
    7 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R+
    8 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 kworker/R+

$ free -m
               total        used        free      shared  buff/cache   available
Mem:            3992         321        3262          12         633        3670
Swap:              0           0           0

$ vmstat 1 5
procs -----------memory---------- ---swap-- -----io---- -system-- -------cpu-------
 r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st gu
 0  0      0 3341088  16004 632332    0    0  1724  5301  268    7 11  3 84  2  0  0
 0  0      0 3341088  16004 632408    0    0     0     0   44   62  0  0 100  0  0  0
 0  0      0 3341088  16004 632412    0    0     0     0   45   57  0  0 100  0  0  0
 0  0      0 3341088  16004 632412    0    0     0     0   42   55  0  0 100  0  0  0
 0  0      0 3341088  16004 632412    0    0     0     0   44   55  0  0 100  0  0  0

$ iostat -x 1 2
Linux 6.18.44-fc-v726 (vm) 	11/21/26 	_x86_64_	(1 CPU)

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
          11.05    0.00    2.90    1.81    0.14   84.10

Device            r/s     rkB/s   rrqm/s  %rrqm r_await rareq-sz     w/s     wkB/s   wrqm/s  %wrqm w_await wareq-sz     d/s     dkB/s   drqm/s  %drqm d_await dareq-sz     f/s f_await  aqu-sz  %util
vda             45.45   1572.90    22.10  32.72    0.25    34.61   56.26   5110.22    11.11  16.50    0.47    90.83    6.70   1771.74     0.00   0.00    0.19   264.26   12.75    0.36    0.04   1.79
vdb              1.65     89.26     0.00   0.00    0.14    54.20    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdc              0.08      0.12     0.00   0.00    0.00     1.44    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdd              0.08      0.12     0.00   0.00    0.00     1.44    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00


avg-cpu:  %user   %nice %system %iowait  %steal   %idle
           0.00    0.00    0.00    0.00    0.00  100.00

Device            r/s     rkB/s   rrqm/s  %rrqm r_await rareq-sz     w/s     wkB/s   wrqm/s  %wrqm w_await wareq-sz     d/s     dkB/s   drqm/s  %drqm d_await dareq-sz     f/s f_await  aqu-sz  %util
vda              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdb              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdc              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdd              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00



$ df -h
Filesystem                                                                         Size  Used Avail Use% Mounted on
tmpfs                                                                              3.9G   92K  3.9G   1% /dev/shm
/dev/vda                                                                           252G  6.9G  9.7G  42% /
model_tools_unavailable                                                            4.0K     0  4.0K   0% /mnt/sandboxing/model_tools_env/v1/python
tmpfs                                                                              2.0G     0  2.0G   0% /sys/fs/cgroup
tmpfs                                                                              1.0M     0  1.0M   0% /run/sandbox-telemetry
rclone-filestore:claude_chat_01MY5hvkcByNpMkuncREsDRU:/mnt/user-data/tool_results  1.0P     0  1.0P   0% /mnt/user-data/tool_results
rclone-filestore:claude_chat_01MY5hvkcByNpMkuncREsDRU:/mnt/user-data/outputs       1.0P     0  1.0P   0% /mnt/user-data/outputs
rclone-filestore:claude_chat_01MY5hvkcByNpMkuncREsDRU:/mnt/user-data/uploads       1.0P     0  1.0P   0% /mnt/user-data/uploads
rclone-filestore:claude_chat_01MY5hvkcByNpMkuncREsDRU:/mnt/transcripts             1.0P     0  1.0P   0% /mnt/transcripts

$ df -i / | head -3
Filesystem       Inodes  IUsed    IFree IUse% Mounted on
/dev/vda       16777216 166487 16610729    1% /

$ ss -tulpn
Netid State  Recv-Q Send-Q Local Address:Port Peer Address:PortProcess
tcp   LISTEN 0      128          0.0.0.0:2024      0.0.0.0:*          

$ ss -s
Total: 30
TCP:   1 (estab 0, closed 0, orphaned 0, timewait 0)

Transport Total     IP        IPv6
RAW	  0         0         0        
UDP	  0         0         0        
TCP	  1         1         0        
INET	  1         1         0        
FRAG	  0         0         0        
```

## 2. Capture under load (portal behind Nginx, `ab -n 6000 -c 20 http://127.0.0.1/health`)

`top` (one snapshot, 3 s into the run):

```text
top - 06:31:58 up 4 min,  0 user,  load average: 0.23, 0.06, 0.02
Tasks:  66 total,   4 running,  62 sleeping,   0 stopped,   0 zombie
%Cpu(s): 72.7 us, 18.2 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  9.1 si,  0.0 st 
MiB Mem :   3992.3 total,   3134.0 free,    379.5 used,    707.0 buff/cache     
MiB Swap:      0.0 total,      0.0 free,      0.0 used.   3612.8 avail Mem 

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
 2334 portal    20   0   39528  30480   8768 R  40.0   0.7   0:01.31 gunicorn
 2335 portal    20   0   39528  30508   8768 R  40.0   0.7   0:01.30 gunicorn
   15 root      20   0       0      0      0 I  10.0   0.0   0:00.04 rcu_preem+
 2345 www-data  20   0   11676   3732   2264 R  10.0   0.1   0:00.45 nginx
 2619 root      20   0   11248   7812   6424 S  10.0   0.2   0:00.25 ab
    1 root      20   0   32756   5652   5116 S   0.0   0.1   0:00.46 process_a+
    2 root      20   0       0      0      0 S   0.0   0.0   0:00.00 kthreadd
```

`vmstat 1` during the run (rows 3–6 are the load; the other rows are idle):

```text
procs -----------memory---------- ---swap-- -----io---- -system-- -------cpu-------
 r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st gu
 4  0      0 3214776  18236 704832    0    0   814  2679  175    4  9  2 88  1  0  0
 0  0      0 3214776  18236 704836    0    0     0   932   73  100  0  0 100  0  0  0
 3  0      0 3213528  18236 705172    0    0     0     0  282 10926 77 22  0  0  1  0
 2  0      0 3211764  18236 705436    0    0     0     0  278 10636 76 22  0  0  2  0
 2  0      0 3210252  18236 705724    0    0     0     0  275 10658 72 26  0  0  2  0
 0  0      0 3207984  18236 706012    0    0     0     0  289 10630 77 21  2  0  0  0
 0  0      0 3207984  18236 706060    0    0     0     8   73   94  0  0 100  0  0  0
 0  0      0 3207984  18236 706060    0    0     0     0   56   64  0  1 99  0  0  0
 0  0      0 3206724  18236 706120    0    0    48     4   83  183  2  0 96  1  1  0
 0  0      0 3206724  18236 706120    0    0     0     0   54   64  0  0 100  0  0  0
 0  0      0 3206724  18236 706120    0    0     0     0   69   82  0  0 100  0  0  0
 0  0      0 3206724  18236 706120    0    0     0     0   56   69  0  0 100  0  0  0
```

`free -m` and `ss -s` during the run:

```text
               total        used        free      shared  buff/cache   available
Mem:            3992         379        3133          12         707        3612
Swap:              0           0           0
```

```text
Total: 116
TCP:   8968 (estab 80, closed 8885, orphaned 0, timewait 8884)

Transport Total     IP        IPv6
RAW	  0         0         0        
UDP	  0         0         0        
TCP	  83        83        0        
INET	  83        83        0        
FRAG	  0         0         0        
```

`ps` sorted by CPU during the run:

```text
USER       PID %CPU %MEM   RSS COMMAND
root      2619  7.7  0.1  7812 ab
portal    2334  2.2  0.7 30480 gunicorn
portal    2335  2.2  0.7 30508 gunicorn
www-data  2345  0.7  0.0  3732 nginx
portal    2333  0.2  0.6 26156 gunicorn
root         1  0.1  0.1  5652 process_api
root      1831  0.1  1.1 46924 containerd
```

`iostat -x 1 2` (second interval, during the run):

```text


avg-cpu:  %user   %nice %system %iowait  %steal   %idle
          60.00    0.00   15.00    0.00    0.00   25.00

Device            r/s     rkB/s   rrqm/s  %rrqm r_await rareq-sz     w/s     wkB/s   wrqm/s  %wrqm w_await wareq-sz     d/s     dkB/s   drqm/s  %drqm d_await dareq-sz     f/s f_await  aqu-sz  %util
vda              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdb              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdc              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
vdd              0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00      0.00     0.00   0.00    0.00     0.00    0.00    0.00    0.00   0.00
```

`ab` result summary (the full file is in `REPORT/B1_Implementation_Report.md`):

```text
This is ApacheBench, Version 2.3 <$Revision: 1903618 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 600 requests
Completed 1200 requests
Completed 1800 requests
Completed 2400 requests
Completed 3000 requests
Completed 3600 requests
Completed 4200 requests
Completed 4800 requests
Completed 5400 requests
Completed 6000 requests
Finished 6000 requests


Server Software:        nginx/1.24.0
Server Hostname:        127.0.0.1
Server Port:            80

Document Path:          /health
Document Length:        16 bytes

Concurrency Level:      20
Time taken for tests:   3.975 seconds
Complete requests:      6000
Failed requests:        0
Total transferred:      1044000 bytes
HTML transferred:       96000 bytes
Requests per second:    1509.52 [#/sec] (mean)
Time per request:       13.249 [ms] (mean)
Time per request:       0.662 [ms] (mean, across all concurrent requests)
Transfer rate:          256.50 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       1
Processing:     2   13   1.9     13      30
Waiting:        2   13   1.9     13      30
Total:          3   13   1.9     13      30

Percentage of the requests served within a certain time (ms)
  50%     13
  66%     13
  75%     14
  80%     14
  90%     16
  95%     17
  98%     19
  99%     20
 100%     30 (longest request)
```

## 3. What the values mean

### Processes (`ps`, `top`)
| Value | Meaning | Observation |
|---|---|---|
| `PID`, `PPID` | Process id and parent id | `process_api` is PID 1 (this micro-VM's init, **not systemd**, see B1.3) |
| `STAT` | `S` sleeping, `R` running, `s` session leader, `l` multi-threaded, `<`/`N` priority | Almost everything is `S`: idle processes wait for events |
| `%CPU`, `%MEM`, `RSS` | CPU share, share of RAM, resident memory | Idle: biggest process was `dockerd` at about 80 MB RSS. Under load the two busy Gunicorn workers (≈40 % CPU each in the `top` snapshot) used about 30 MB RSS each |
| `Tasks` (top) | Total/running/sleeping | 55 tasks idle, 66 under load (Nginx, Gunicorn, cron, `ab`) |
| `load average` | Average number of runnable (+ uninterruptible) tasks over 1, 5, 15 min | 0.04 idle; 0.23 (1-minute) right after the burst. On a **1-CPU** machine a sustained value above 1.0 would mean queuing |
| `%Cpu(s)` `us/sy/id/wa/si/st` | User, kernel, idle, I/O-wait, soft-interrupt, steal | Idle 100 % id; under load 72.7 % user, 18.2 % system, 9.1 % soft-IRQ (network packets), **0 % wa** → CPU-bound, not disk-bound |

### Memory (`free`, `vmstat`)
| Value | Meaning | Observation |
|---|---|---|
| `total` / `used` / `free` | Installed RAM / in use by programs / completely unused | 3992 / 321 / 3262 MiB at idle |
| `buff/cache` | Page cache and buffers; the kernel uses spare RAM for file caching and gives it back when needed | 633 MiB idle → 707 MiB under load (log files being written) |
| `available` | Estimate of RAM available to start new programs without swapping (`free` + reclaimable cache) | 3670 MiB: this, not `free`, is the number to watch |
| `Swap` | Disk used as overflow memory | 0 total: no swap configured; an OOM-kill would be the failure mode, not slow swapping |
| `vmstat si/so` | Pages swapped in/out per second | 0 → no memory pressure |
| `vmstat swpd, free, buff, cache` | Same quantities in KiB | Free memory fell only ≈ 8 MB during the run |

### CPU and scheduling (`vmstat`)
| Column | Meaning | Observation |
|---|---|---|
| `r` | Runnable processes waiting for a CPU | 2–3 during the load (rows 3–6): more runnable tasks than the 1 CPU, i.e. the CPU is the bottleneck |
| `b` | Processes blocked in uninterruptible sleep (usually disk) | 0 |
| `in`, `cs` | Interrupts and context switches per second | ≈ 280 and ≈ 10,700 during load vs ≈ 45 and ≈ 60 idle: every request causes several switches between `ab`, Nginx and Gunicorn |
| `us sy id wa st` | CPU time split (user, system, idle, I/O wait, stolen by hypervisor) | 72–77 us, 21–26 sy, 0 id during load; `st` 0–2 % because the VM shares the host CPU |
| First `vmstat` row | Averages **since boot**, not the current second | 84 % idle there reflects the boot activity |

### Disk (`iostat`, `df`)
| Value | Meaning | Observation |
|---|---|---|
| `r/s`, `w/s`, `rkB/s`, `wkB/s` | I/O requests and throughput per second | Since boot (first `iostat` block, `vda`): 45 reads/s, 56 writes/s, about 5 MB/s written (boot-time activity). During the load: 0 – the portal only writes small log lines, which stay in the page cache |
| `r_await`, `w_await` (ms) | Average wait per request | 0.25 and 0.47 ms: fast virtual disk |
| `%util` | Fraction of time the device was busy | 1.8 %: nowhere near saturated |
| `avg-cpu %iowait` | CPU idle while I/O was pending | 1.81 % since boot, 0 under load |
| `df -h` `Size/Used/Avail/Use%` | File-system capacity and use | `/` is at 42 %. **Note:** in this sandbox `Size` (252G) does not equal `Used + Avail` (6.9G + 9.7G) because the virtual disk is thin-provisioned; on a normal server they add up. The alert script uses the `Use%` column |
| `df -i` | Inode (file-count) use | 1 % of 16.7 M inodes: inode exhaustion can fill a disk even when bytes are free |

### Network (`ss`)
| Value | Meaning | Observation |
|---|---|---|
| `ss -tulpn` | Listening TCP/UDP sockets and their owning process | Idle: only port 2024 (the sandbox's own control channel). After the portal was started (B1.3): `0.0.0.0:80` owned by Nginx and `127.0.0.1:8000` owned by Gunicorn, i.e. Gunicorn is **not** reachable from outside |
| `Recv-Q`/`Send-Q` on a LISTEN socket | Send-Q = backlog size | Nginx 511, Gunicorn 2048 |
| `ss -s` `estab` | Established connections | 80 during the run |
| `timewait` | Closed connections the kernel keeps for about 60 s | **8,884** after 6,000 requests: `ab` opens a new connection per request (no keep-alive), and two hops (client→Nginx, Nginx→Gunicorn) double the count. On a real result day this is why keep-alive, `net.ipv4.ip_local_port_range` and connection reuse matter |

## 4. Summary of the findings
1. At idle the machine uses about 8 % of its RAM, no swap, no disk pressure and no CPU.
2. Under load the **CPU is the first resource to saturate** (0 % idle, run-queue above 1), not memory or disk. That supports the Part A design decision to scale the number of Gunicorn workers/containers with CPU cores.
3. Per Gunicorn worker the memory cost is about 30 MB, so memory is not the limit for this simple application.
4. Measured throughput of the `/health` route through Nginx on **one shared vCPU was about 1,510 requests/s**, with no failed requests. This is a measurement of a trivial route, not of a real database-backed portal, and must not be used for capacity planning without the B3/B4 measurements.
