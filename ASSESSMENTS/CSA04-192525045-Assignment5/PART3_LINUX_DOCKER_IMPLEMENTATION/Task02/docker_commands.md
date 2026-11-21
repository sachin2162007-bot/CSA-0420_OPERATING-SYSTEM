# B2 – Docker Commands and Evidence Status

Evidence labels: **ACTUAL** = produced in this session; **PENDING** = not executed, command given; **EXPECTED** = what the official PDF says should happen (not a measurement).

## 1. What could and could not be executed

| Step | Status | Reason |
|---|---|---|
| Install Docker Engine 29.1.3 and Compose 2.40.3 in the sandbox | ACTUAL | `apt-get install docker.io docker-compose-v2` |
| Start the Docker daemon (`dockerd`) | ACTUAL | Started; `API listen on /var/run/docker.sock` |
| `docker compose config -q` (validates `docker-compose.yml`) | ACTUAL | Valid, exit 0 |
| `docker build -t result-portal .` | **ACTUAL FAILURE: blocked, image NOT built** | The base image `python:3.12-slim` cannot be downloaded: Docker Hub (`registry-1.docker.io`) cannot be resolved from the sandbox |
| `docker run`, `docker ps`, `docker images`, `docker exec … whoami`, `docker stats`, `docker history`, Compose run, volume test, image-size comparison | **PENDING** | They all need the image |

ACTUAL output of the attempt (unedited):

```text
$ docker --version
Docker version 29.1.3, build 29.1.3-0ubuntu3~24.04.2
$ docker compose version
Docker Compose version 2.40.3+ds1-0ubuntu1~24.04.1
$ docker build -t result-portal .
DEPRECATED: The legacy builder is deprecated and will be removed in a future release.
            Install the buildx component to build images with BuildKit:
            https://docs.docker.com/go/buildx/

Sending build context to Docker daemon  13.31kB

Step 1/9 : FROM python:3.12-slim
failed to resolve reference "docker.io/library/python:3.12-slim": failed to do request: Head "https://registry-1.docker.io/v2/library/python/manifests/3.12-slim": dial tcp: lookup registry-1.docker.io on 8.8.8.8:53: no such host
[exit status: 1]
$ docker images
WARNING: This output is designed for human readability. For machine-readable output, please use --format.
IMAGE   ID             DISK USAGE   CONTENT SIZE   EXTRA
$ docker compose config -q
[exit status: 0]
```

(The legacy-builder warning appears because the `buildx` plug-in is not installed in the sandbox. The build stops at the first instruction, `FROM python:3.12-slim`; `docker images` is empty, proving that no image exists.)

**Nothing below this section has been run. No image size, no `docker stats` figure and no container output is claimed.** To produce the real evidence on any machine with Docker and Internet access:

```bash
cd Task02
bash capture_evidence.sh      # runs every command below, writes real output to Task02/evidence_actual/
```

## 2. Commands (all PENDING)

### 2.1 Build, run, verify (official commands)
```bash
docker build -t result-portal .
docker run -p 8000:8000 result-portal            # foreground, as in the PDF
# in a second terminal, or use -d instead:
docker ps                                        # EXPECTED: container "Up", 0.0.0.0:8000->8000/tcp
docker images                                    # record the SIZE of result-portal
docker exec <container> whoami                   # EXPECTED: portal
curl http://localhost:8000/                      # EXPECTED: "University Result Portal" page
curl http://localhost:8000/health                # EXPECTED: {"status":"ok"}
```
What each shows about containers: `docker build` executes each Dockerfile instruction and stores the result as a read-only **layer**; `docker run -p 8000:8000` starts a process in new **namespaces** and connects host port 8000 to container port 8000 (NAT); `docker exec … whoami` shows the process runs as an unprivileged user inside the container (UID 1000 is mapped to the same UID on the host, so non-root inside means non-root outside unless user namespaces are used).

### 2.2 Compose with a volume
```bash
docker compose up -d --build
docker compose ps
curl http://localhost:8000/results               # "source": "postgresql"
docker volume ls | grep pgdata
docker compose exec db psql -U portal -d results -c "INSERT INTO results VALUES ('192525099','CSA04','C');"
docker compose down                              # containers removed, volume kept
docker compose up -d
curl http://localhost:8000/results               # the inserted row must still be there (volume persisted)
docker compose down -v                           # also removes the volume
```
Meaning: the container's writable layer is deleted with the container; the **named volume** `pgdata` lives outside it, so the database files survive `down` and `up`. `down -v` deletes the volume, showing the difference.

### 2.3 Limits and `docker stats` → see [resource_limits.md](resource_limits.md)
### 2.4 Layers and size reduction → see [image_optimisation.md](image_optimisation.md)

## 3. Static verification done instead (ACTUAL, not Docker)
The application code that goes into the image was run outside Docker with the same start command (`gunicorn --bind … app:app`) as the unprivileged user `portal`, with Python 3.12.3 and the pinned requirements. Routes `/`, `/results`, `/health` returned the expected responses (see `REPORT/B1_Implementation_Report.md` section 3.1). This shows that `app.py`, `requirements.txt` and the `app:app` object are consistent with the Dockerfile `CMD`; it does **not** show that the image builds or that the container runs.

The database path was also checked outside Docker with a local PostgreSQL 16 server (the Compose file uses `postgres:16-alpine`): `database/init.sql` created and filled the table, and the application, started with `DATABASE_URL` set, reported `"source":"postgresql"` (ACTUAL):

```text
$ psql ... -c "SELECT * FROM results ORDER BY reg_no;"
  reg_no   | course | grade 
-----------+--------+-------
 192525001 | CSA04  | A
 192525002 | CSA04  | B
 192525045 | CSA04  | A
(3 rows)

$ DATABASE_URL=postgresql://portal:***@127.0.0.1:5432/results  (app run on port 8011 with the project venv, outside Docker)
$ curl -s http://127.0.0.1:8011/results
{"results":[{"course":"CSA04","grade":"A","reg_no":"192525001"},{"course":"CSA04","grade":"B","reg_no":"192525002"},{"course":"CSA04","grade":"A","reg_no":"192525045"}],"source":"postgresql"}
```
