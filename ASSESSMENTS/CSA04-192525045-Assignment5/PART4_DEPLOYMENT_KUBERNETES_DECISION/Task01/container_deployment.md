# B3.1 – Container deployment

The unmodified Part 3 `Dockerfile` (TODO 1–3 already completed there) was built and tagged for Part 4:
```bash
cd PART3_LINUX_DOCKER_IMPLEMENTATION/Task02
docker build -t result-portal -t simats/result-portal:1.0 .
docker run -d --name rp -p 8000:8000 simats/result-portal:1.0
curl localhost:8000/health        # {"status":"ok"}
docker exec rp whoami             # portal
```
ACTUAL result: build succeeded (4.7 s with warm layers), health returned `{"status":"ok"}`, `whoami` printed `portal`.

**Substitution, stated plainly:** Docker Hub is unreachable from the sandbox, so `python:3.12-slim` could not be pulled. A local stand-in
(Ubuntu 24.04 minbase via debootstrap + python3.12 + pip, imported with `docker import`) was tagged `python:3.12-slim` so the Part 3
Dockerfile ran without edits. Consequently the **image size is not the size of the real python:3.12-slim image**. On your machine, run
`docker images simats/result-portal` and replace the size figure.
