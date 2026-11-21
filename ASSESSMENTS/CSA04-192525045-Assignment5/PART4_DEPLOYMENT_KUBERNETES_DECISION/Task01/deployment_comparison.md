# B3.4 – Engineering trade-off

## Measured comparison (means of 3 runs; `ab -n 5000 -c 50 /results`; 1 vCPU host)

| Metric | Bare metal | Virtual machine | Container (docker run) | Kubernetes (3 pods, ClusterIP) |
|---|---|---|---|---|
| Start-up to first 200 on /health | 139 ms | PENDING | 318 ms | not measured (pod Running about 3 s after delete, see Task02) |
| Memory (idle / loaded) | 55.1 / 55.2 MiB (RSS) | PENDING | 31.1 / 31.3 MiB (cgroup) | 31 MiB per pod (`kubectl top`, idle) |
| Disk / image | 36 MiB (venv + app) | PENDING | 62.3 MB content, 243 MB on disk (stand-in base) | same image |
| Requests per second | 5,190 | PENDING | 2,006 | 2,555 (3 pods share 1 vCPU) |
| Mean latency per request / p95 | 9.6 / 10 ms | PENDING | 24.9 / 26 ms | p95 43–47 ms |

## Explanation
* Bare metal is fastest and starts fastest because there is no extra layer: the process runs on the host kernel and loopback.
* The container shares the host kernel, so its CPU path is the same code; the measured gap (about 2.6x fewer rps) comes mostly from the
  Docker proxy/NAT path and the cgroup accounting on a single shared vCPU, plus roughly 180 ms of extra start-up (`docker run` create+start). It is a measurement of this
  setup, not a general law. Container memory looks lower only because cgroup memory counts shared pages once while summed RSS counts them twice.
* Kubernetes adds scheduling, kube-proxy and the Service hop; with 3 pods on one vCPU there is no extra CPU to gain, so throughput stays
  near the single-container level. Kubernetes' value is placement, self-healing and scaling across nodes, not speed on one core.
* A VM would add a full guest OS (memory in the hundreds of MB and boot in tens of seconds is the usual order of magnitude) – to be
  confirmed by running `vm_deployment.md`; no number is claimed here.

## When to use each
| Method | Use when |
|---|---|
| Bare metal | One dedicated, performance-critical service; lowest overhead; few instances; you accept manual admin and weak isolation. |
| Virtual machine | Strong isolation between tenants/departments, different guest OSes, snapshots and live migration, legacy software. |
| Container (Docker) | Same image everywhere, fast start, many small services per server, CI/CD; single host. |
| Kubernetes | Several nodes, self-healing, rolling updates, scaling for a spike such as result day, mixed services (portal, LMS, exams). |
