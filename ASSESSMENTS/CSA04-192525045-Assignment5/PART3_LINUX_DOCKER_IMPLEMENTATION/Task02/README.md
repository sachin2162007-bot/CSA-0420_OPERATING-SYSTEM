# Task 02 – B2 Containerise the Portal (5 marks)

| Name | S. Sachin |
|---|---|
| Register number | 192525045 |
| Course | CSA04 Operating Systems, B.Tech AI and ML, SIMATS Engineering |
| Application | Python 3.12 + Flask 3.1.3 + Gunicorn 27.0.1 + psycopg2-binary 2.9.13 (`requirements.txt`) |
| Container tooling installed in the sandbox | Docker Engine 29.1.3, Docker Compose 2.40.3 on Ubuntu 24.04.5 (kernel 6.18.44) |

## Honest status
**The image has not been built or run**, because the sandbox cannot reach Docker Hub to download `python:3.12-slim` (real error in `docker_commands.md`). Therefore every Docker result (image size, `docker ps`, `whoami`, `docker stats`, `docker history`, Compose, volume test, optimisation measurements) is **PENDING**. One command produces all of it on a Docker-enabled machine: `bash capture_evidence.sh`.

Done and verified: official Dockerfile completed (3 TODOs); `docker-compose.yml` validated (`docker compose config -q` → valid); the application started by Gunicorn as an unprivileged user and answered `/`, `/results`, `/health` outside Docker.

## Files
| File | Content |
|---|---|
| `Dockerfile` | Official page-47 Dockerfile with TODO 1–3 completed; all other lines unchanged |
| `app.py`, `requirements.txt` | The Flask application and pinned dependencies |
| `docker-compose.yml` | portal + PostgreSQL, named volume `pgdata`, `cpus`/`mem_limit` on both services |
| `database/init.sql` | Creates and fills the `results` table on first start |
| `.dockerignore` | Keeps the build context small |
| `Dockerfile.multistage`, `Dockerfile.ubuntu` | Comparison variants for the "Alternative Concept" (not the official file) |
| `capture_evidence.sh` | Runs every B2 command and stores real output in `evidence_actual/` |
| `docker_commands.md`, `resource_limits.md`, `image_optimisation.md` | Commands, expectations, empty result tables (PENDING) |
| `expected_output.txt` | The PDF's expected result vs the actual status |

## The completed TODOs (Dockerfile)
```dockerfile
# TODO 1:  WORKDIR /app
# TODO 2:  RUN useradd --create-home --shell /usr/sbin/nologin portal
#          COPY --chown=portal:portal . .
#          USER portal
# TODO 3:  EXPOSE 8000
#          CMD ["gunicorn", "--bind", "0.0.0.0:8000", "app:app"]
```
`useradd` comes before `COPY --chown` because the user must exist before it can own files; `USER portal` is the last step before `CMD` so that `pip install` still runs as root but the application does not.

## How to run (on a machine with Docker)
```bash
docker build -t result-portal .
docker run -p 8000:8000 result-portal
docker compose up -d --build          # portal + database + volume
bash capture_evidence.sh              # everything, with real output saved
```
Compiler line from the assignment template (gcc) does not apply: this task has no C code.

## Note on `.dockerignore`
It excludes `*.md`, `Dockerfile*`, `docker-compose.yml` and `database/` from the **image**; Compose still mounts `database/init.sql` into the database container because bind mounts are not affected by `.dockerignore`.
