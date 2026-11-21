# Measurement methodology (B3.2)

**Hardware / OS (ACTUAL):** Intel Xeon @ 2.30 GHz, 1 vCPU, 3992 MiB RAM, disk 252 GB (≈10 GB free), Ubuntu 24.04.5 LTS, kernel 6.18.44,
the host is itself a KVM guest (micro-VM). Python 3.12.3, Flask 3.1.3, gunicorn 27.0.1, Docker 29.1.3 (cgroup v1), ab 2.3 (apache2-utils 2.4.58).

**Load:** `ab -n 5000 -c 50 <url>/results`, no keep-alive, 3 runs per deployment, 0 failed requests in every run. ab runs on the same single vCPU as the server.

| Metric | How measured |
|---|---|
| Start-up time | ms from launching the process / `docker run -d` until `/health` first returns 200 (curl polled every 20 ms). Container figure includes Docker's create+start. |
| Memory | Bare: sum of RSS of gunicorn master+worker (`ps`), after 1 s idle and after load. Container: `docker stats --no-stream` MemUsage (cgroup). |
| Disk / image | Bare: `du -sm` of the venv + app.py (system Python excluded). Container: `docker image inspect .Size` (62.3 MB content; `docker images` shows 243 MB on disk). |
| Requests/s | ab "Requests per second"; mean and 95th percentile latency recorded. |

**Limits (do not over-interpret):**
1. RSS double-counts pages shared by master and worker; cgroup memory does not. The two memory numbers are not like-for-like.
2. Client and server share one vCPU, so absolute rps is lower than on separate machines; ratios between rows are the useful part.
3. The container row goes through Docker's userland proxy/NAT on that one vCPU; part of the throughput gap is network path, not containers as such.
4. The image size reflects a local stand-in base image (see `container_deployment.md`).
5. App serves built-in sample rows (no database); results overstate a database-backed portal.
6. VM row: not measured.
