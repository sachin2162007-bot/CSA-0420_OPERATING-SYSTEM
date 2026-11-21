# B2 – Limiting CPU and Memory, and `docker stats`

**Status: PENDING.** The image could not be built in the sandbox (Docker Hub unreachable), so no `docker stats` output exists. Commands and the *reasoned* expectation are given; the expectation is derived from how cgroups work, not from a measurement. `capture_evidence.sh` (step 04) runs everything and stores the real output.

## Compose limits (in `docker-compose.yml`, validated with `docker compose config -q`)
| Service | `cpus` | `mem_limit` |
|---|---|---|
| portal | 0.50 | 256m |
| db | 0.50 | 256m |

## Experiment (PENDING)
```bash
docker run -d --name p_free -p 8001:8000 result-portal                          # no limits
docker run -d --name p_lim  -p 8002:8000 --cpus 0.5 --memory 128m result-portal # limited
docker inspect -f '{{.Name}} NanoCpus={{.HostConfig.NanoCpus}} Memory={{.HostConfig.Memory}}' p_free p_lim
docker stats --no-stream p_free p_lim                      # idle
docker exec -d p_free python -c "while True: pass"         # one busy thread in each
docker exec -d p_lim  python -c "while True: pass"
sleep 8; docker stats --no-stream p_free p_lim
docker exec p_lim  python -c 'b = bytearray(300*1024*1024); print(len(b))'   # 300 MB in a 128 MB container
docker inspect -f '{{.State.OOMKilled}}' p_lim
```

## Expected behaviour (reasoning, NOT results)
| Observation | Why it should happen |
|---|---|
| `NanoCpus=500000000`, `Memory=134217728` | `--cpus 0.5` = 0.5 × 10⁹ nano-CPUs; `--memory 128m` = 128 × 1024² bytes |
| Busy loop: `p_free` near 100 % CPU, `p_lim` near 50 % | `--cpus 0.5` sets the CFS bandwidth quota to 50 ms of CPU time per 100 ms period (`cpu.max` = `50000 100000`); once used up, the container's threads are throttled until the next period. One busy thread would otherwise use 100 % of one core |
| `docker stats` MEM USAGE / LIMIT shows 128 MiB as the limit for `p_lim`; the host's total memory for `p_free` | The `LIMIT` column reads `memory.max` of the container's cgroup |
| 300 MB allocation in `p_lim` is killed (exit status 137 = 128 + SIGKILL); `OOMKilled=true` may be reported | A container may not exceed `memory.max`; with no swap allowance the kernel OOM-killer terminates the process. The same allocation succeeds in `p_free` if the host has the RAM |
| The host and other containers stay healthy | The failure is confined to the container's cgroup: the isolation benefit of limits |

Caveats to state when real numbers are available: `docker stats` percentages are relative to **one core** (200 % = two cores); on a 1-vCPU machine `p_free` cannot exceed 100 %; values are a snapshot, not an average.

## What this step shows about how containers work
Containers are ordinary Linux processes. **Namespaces** give them their own view of the system, **cgroups** limit how much CPU, memory and I/O they may consume. Docker just writes the numbers into the cgroup files. This is the same mechanism systemd uses for `MemoryMax=` in `portal.service` and Kubernetes uses for `resources.limits` in B3.
