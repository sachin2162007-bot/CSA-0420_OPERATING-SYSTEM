# B2 – Containerise the Portal: Report (5 marks)

Student: S. Sachin | Register number: 192525045 | Course: CSA04 Operating Systems | B.Tech AI and ML, SIMATS Engineering

Evidence labels: **ACTUAL** = run in this session; **PENDING** = not run, command given; **EXPECTED** = from the official PDF or derived by reasoning, never a measurement.

## 1. Summary of status

| Required item | Status |
|---|---|
| 1. Dockerfile for the portal (official page-47 file, TODO 1–3) | **Done** (`Task02/Dockerfile`) |
| 2. Build the image | **PENDING**: blocked, `python:3.12-slim` could not be pulled (Docker Hub unreachable); real error recorded |
| 3. `docker run -p 8000:8000`, `docker ps`, `docker images`, `docker exec … whoami` | **PENDING** |
| 4. Docker Compose with the database | File written and validated (`docker compose config -q` → valid, ACTUAL); run **PENDING** |
| 5. Volume for database data | In `docker-compose.yml` (`pgdata`); persistence test **PENDING** |
| 6. CPU and memory limits | In `docker-compose.yml` and `--cpus/--memory` commands; **PENDING** |
| 7. Effect shown with `docker stats` | **PENDING** (no numbers claimed) |
| 8. `docker history` | **PENDING** |
| 9. Reduce image size | Measures built in (slim, `--no-cache-dir`, `.dockerignore`, `--chown`, multi-stage file); sizes **PENDING** |

The **only** thing that stops items 2–9 is network access to the image registry; the files are ready, and `bash Task02/capture_evidence.sh` produces all evidence in one run on a Docker-enabled computer.

## 2. Environment
Ubuntu 24.04.5 LTS micro-VM (kernel 6.18.44, 1 vCPU, 3992 MiB); Docker Engine 29.1.3 and Compose 2.40.3 installed with `apt` and the daemon started; the legacy builder was used (BuildKit's `buildx` plug-in not installed). Python 3.12.3, Flask 3.1.3, Gunicorn 27.0.1, psycopg2-binary 2.9.13, PostgreSQL 16 (local test only).

## 3. The Dockerfile (official code with the three TODOs completed)
```dockerfile
# Container image for the campus result-portal web service (a small Flask app).
FROM python:3.12-slim
# TODO 1: Set the working directory inside the image to /app
WORKDIR /app
# Install the dependencies first so that this layer is cached between builds.
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
# TODO 2: Copy the application source into the image and run it as a non-root user named
# portal
RUN useradd --create-home --shell /usr/sbin/nologin portal
COPY --chown=portal:portal . .
USER portal
# TODO 3: Document that the service listens on port 8000 and start it with gunicorn on
# 0.0.0.0:8000 using the app object in app.py
EXPOSE 8000
CMD ["gunicorn", "--bind", "0.0.0.0:8000", "app:app"]
```
All original lines and comments are unchanged; only the lines after each TODO comment were added.

| TODO | Instruction | Why |
|---|---|---|
| 1 | `WORKDIR /app` | Sets the directory for the following `COPY`, `RUN` and for the running process; created if missing |
| 2 | `useradd`, `COPY --chown`, `USER portal` | If the application is compromised, the attacker is not root in the container. The user is created first so it can own the copied files; `pip install` above still runs as root (it writes to system directories) |
| 3 | `EXPOSE 8000` | **Documentation only**: it records the port in the image metadata; `docker run -p 8000:8000` is what publishes it. `CMD` in exec form (JSON array) runs Gunicorn as PID 1 without a shell, so `docker stop` signals reach it directly. `app:app` = module `app.py`, object `app` |

`gunicorn --bind 0.0.0.0:8000` inside the container is correct (binding to 127.0.0.1 inside a container would be unreachable through the port mapping), unlike the loopback-only binding used behind Nginx in B1.

## 4. What was verified without the image (ACTUAL)
- `docker compose config -q` → valid (exit 0). `docker build` attempt → fails at `FROM` with the registry-resolution error shown in `Task02/docker_commands.md`.
- The same `gunicorn … app:app` command served `/`, `/health` and `/results` as the non-root user `portal` (B1 report, section 3.1).
- `database/init.sql` and the PostgreSQL-backed `/results` route worked against a local PostgreSQL 16 (output in `Task02/docker_commands.md`, section 3).
These checks prove the application, dependency list and SQL are sound; they are **not** container evidence.

## 5. Commands and interpretation (PENDING, expected behaviour reasoned)
See `Task02/docker_commands.md`, `resource_limits.md`, `image_optimisation.md`. Summary of what each step is meant to demonstrate about containers:

| Step | What it shows |
|---|---|
| `docker build` / `docker history` | An image is a stack of read-only layers, one per file-changing instruction; the cache is invalidated from the first changed layer downward, hence dependencies before source |
| `docker run -p` | A container is a process in its own namespaces (PID, mount, network, UTS, IPC); the port mapping is NAT from host to container |
| `docker exec … whoami` → `portal` | The `USER` instruction is enforced; least privilege |
| Compose + volume | Containers are disposable, state must live outside the writable layer; the named volume survives `down` |
| `--cpus 0.5 --memory 128m` and `docker stats` | Limits are cgroup settings (CFS quota, `memory.max`); a container over its memory limit is OOM-killed without harming the host |
| Image-size comparison | Size, pull time and package count depend mostly on the base image; multi-stage helps only when build-time tooling would otherwise end up in the final image |

## 6. Alternative concept: Ubuntu base vs slim vs multi-stage
See [`alternative_analysis.md`](alternative_analysis.md) (qualitative, with PENDING measurement table) and [`complexity_analysis.md`](complexity_analysis.md).

## 7. Pending evidence for B2 (all of it)
Build output and image size; `docker ps`/`docker images`/`whoami`; browser or `curl` result at `http://localhost:8000`; `docker stats` with and without limits; `docker history`; Compose start and volume persistence; the three-variant size/time/package/pull-time table. No screenshots were created or are claimed.

## 8. Known issues and risks to check when the build runs
1. `.dockerignore` excludes `*.md`, `database/`, `docker-compose.yml`, `Dockerfile*`; if the build complains about a missing file, check this list first.
2. `useradd` is available in `python:3.12-slim` (Debian); it would not be in an Alpine-based image.
3. The Compose database password is a demonstration value; real deployments use secrets.
4. Docker's published ports bypass `ufw` rules (Docker writes its own iptables rules), relevant if the B1 firewall is active on the same host.
