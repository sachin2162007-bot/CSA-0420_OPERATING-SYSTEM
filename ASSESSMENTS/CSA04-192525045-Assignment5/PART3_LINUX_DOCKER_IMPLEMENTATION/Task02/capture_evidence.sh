#!/usr/bin/env bash
# Runs every B2 command on a machine that has Docker and Internet access to Docker Hub, and
# stores the REAL output in Task02/evidence_actual/. Run it from the Task02 directory:
#     bash capture_evidence.sh
# It was NOT executed for this submission because Docker Hub was unreachable from the sandbox
# (see docker_commands.md, section 1). Nothing it would print has been invented.
set -uo pipefail
cd "$(dirname "$0")"
OUT=evidence_actual; mkdir -p "$OUT"

step() {                                   # step <file> <command string>
    local f="$OUT/$1"; shift
    { echo "\$ $*"; bash -c "$*" 2>&1; echo "[exit status: $?]"; echo; } | tee -a "$f"
}
: > "$OUT/01_environment.txt" 
step 01_environment.txt "date; uname -srm; grep PRETTY_NAME /etc/os-release; docker --version; docker compose version"

# ---- 1. build the official Dockerfile --------------------------------------------------
step 02_build.txt "time docker build -t result-portal ."
step 02_build.txt "docker images result-portal"

# ---- 2. run with port mapping, verify --------------------------------------------------
docker rm -f portal1 >/dev/null 2>&1
step 03_run.txt "docker run -d --name portal1 -p 8000:8000 result-portal"
sleep 3
step 03_run.txt "docker ps"
step 03_run.txt "docker images"
step 03_run.txt "docker exec portal1 whoami"
step 03_run.txt "docker exec portal1 id"
step 03_run.txt "curl -si http://localhost:8000/health"
step 03_run.txt "curl -s http://localhost:8000/"
step 03_run.txt "docker logs portal1 | tail -5"
step 03_run.txt "docker stop portal1 && docker rm portal1"

# ---- 3. CPU / memory limits and docker stats -------------------------------------------
docker rm -f p_free p_lim >/dev/null 2>&1
step 04_limits.txt "docker run -d --name p_free -p 8001:8000 result-portal"
step 04_limits.txt "docker run -d --name p_lim  -p 8002:8000 --cpus 0.5 --memory 128m result-portal"
sleep 3
step 04_limits.txt "docker inspect -f '{{.Name}} NanoCpus={{.HostConfig.NanoCpus}} Memory={{.HostConfig.Memory}}' p_free p_lim"
step 04_limits.txt "docker stats --no-stream p_free p_lim      # idle"
# one busy loop in each container: the unlimited one can use a full core, the limited one only half
docker exec -d p_free python -c "while True: pass"
docker exec -d p_lim  python -c "while True: pass"
sleep 8
step 04_limits.txt "docker stats --no-stream p_free p_lim      # CPU busy loop in both"
# memory limit: allocate 300 MB inside a container limited to 128 MB (expected: killed, exit 137)
step 04_limits.txt "docker exec p_lim python -c 'b = bytearray(300*1024*1024); print(len(b))'"
step 04_limits.txt "docker exec p_free python -c 'b = bytearray(300*1024*1024); print(len(b))'"
step 04_limits.txt "docker inspect -f '{{.Name}} OOMKilled={{.State.OOMKilled}} Status={{.State.Status}}' p_lim"
docker rm -f p_free p_lim >/dev/null 2>&1

# ---- 4. layers ---------------------------------------------------------------------------
step 05_layers.txt "docker history result-portal"
step 05_layers.txt "docker history --no-trunc --format '{{.Size}}\t{{.CreatedBy}}' result-portal | head -20"

# ---- 5. image-size comparison ---------------------------------------------------------------
step 06_optimisation.txt "time docker build --no-cache -f Dockerfile.multistage -t result-portal:multistage ."
step 06_optimisation.txt "time docker build --no-cache -f Dockerfile.ubuntu     -t result-portal:ubuntu ."
step 06_optimisation.txt "time docker build --no-cache -t result-portal:slim ."
step 06_optimisation.txt "docker images --format 'table {{.Repository}}:{{.Tag}}\t{{.Size}}' | grep result-portal"
for img in result-portal:slim result-portal:multistage result-portal:ubuntu; do
    step 06_optimisation.txt "echo $img; docker run --rm --entrypoint sh $img -c 'dpkg -l | grep -c ^ii; pip list | wc -l'"
done
# pull time of the base images (removes them locally first; needs the registry)
step 06_optimisation.txt "docker rmi python:3.12-slim ubuntu:24.04 >/dev/null 2>&1; time docker pull python:3.12-slim"
step 06_optimisation.txt "time docker pull ubuntu:24.04"

# ---- 6. Docker Compose with a volume ---------------------------------------------------------
step 07_compose.txt "docker compose up -d --build"
sleep 10
step 07_compose.txt "docker compose ps"
step 07_compose.txt "curl -s http://localhost:8000/results"
step 07_compose.txt "docker volume ls | grep pgdata"
step 07_compose.txt "docker stats --no-stream \$(docker compose ps -q)"
step 07_compose.txt "docker compose exec db psql -U portal -d results -c \"INSERT INTO results VALUES ('192525099','CSA04','C');\""
step 07_compose.txt "docker compose down"                      # containers removed, volume kept
step 07_compose.txt "docker compose up -d"
sleep 10
step 07_compose.txt "curl -s http://localhost:8000/results     # the 4th row must still be there"
step 07_compose.txt "docker compose down -v"                  # -v also deletes the volume (clean-up)
echo; echo "Done. Real output is in $OUT/. Copy the relevant parts into docker_commands.md and the reports."
