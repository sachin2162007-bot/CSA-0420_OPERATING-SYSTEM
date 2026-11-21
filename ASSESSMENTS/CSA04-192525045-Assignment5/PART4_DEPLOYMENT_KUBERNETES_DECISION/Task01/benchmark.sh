#!/usr/bin/env bash
# B3.2 benchmark: same Flask app (Part 3, Task02/app.py), same gunicorn command, three deployments.
# Usage:  bash benchmark.sh bare        # directly on Linux (python venv + gunicorn)
#         bash benchmark.sh container   # docker run of the Part 3 image simats/result-portal:1.0
#         bash benchmark.sh vm <url>    # app already running inside a VM, e.g. http://192.168.56.10:8000
# Env:    REQS (default 5000)  CONC (default 50)  RUNS (default 3)  PATH_UNDER_TEST (default /results)
set -euo pipefail
MODE=${1:?mode: bare|container|vm}
VM_URL=${2:-}
HERE=$(cd "$(dirname "$0")" && pwd)
APP_DIR=${APP_DIR:-$HERE/../../PART3_LINUX_DOCKER_IMPLEMENTATION/Task02}
REQS=${REQS:-5000}; CONC=${CONC:-50}; RUNS=${RUNS:-3}; P=${PATH_UNDER_TEST:-/results}
CSV=${CSV:-$HERE/benchmark_results.csv}
[ -f "$CSV" ] || echo "mode,run,startup_ms,idle_mem_mib,loaded_mem_mib,disk_mib,requests,concurrency,rps,mean_ms,p95_ms,failed" > "$CSV"

now_ms()  { echo $(( $(date +%s%N) / 1000000 )); }
wait_up() { until curl -fs -o /dev/null "$1/health"; do sleep 0.02; done; }   # polls every 20 ms
sum_rss_mib() { ps -o rss= --ppid "$1" -p "$1" | awk '{s+=$1} END {printf "%.1f", s/1024}'; }  # master + workers
ab_run() {  # $1 = base url
  ab -n "$REQS" -c "$CONC" "$1$P" > /tmp/ab_last.txt 2>&1
  RPS=$(awk '/Requests per second/ {print $4}' /tmp/ab_last.txt)
  MEAN=$(awk '/Time per request/ && !/across/ {print $4}' /tmp/ab_last.txt)
  P95=$(awk '$1=="95%" {print $2}' /tmp/ab_last.txt)
  FAIL=$(awk '/Failed requests/ {print $3}' /tmp/ab_last.txt)
}

case $MODE in
bare)
  BASE=http://127.0.0.1:8001
  VENV=/tmp/portal-venv; rm -rf "$VENV"; python3 -m venv "$VENV"
  "$VENV/bin/pip" install -q --no-cache-dir -r "$APP_DIR/requirements.txt"
  DISK=$(du -sm "$VENV" "$APP_DIR/app.py" | awk '{s+=$1} END {print s}')   # venv + app, excludes system python
  for r in $(seq 1 "$RUNS"); do
    t0=$(now_ms)
    (cd "$APP_DIR" && exec "$VENV/bin/gunicorn" --bind 127.0.0.1:8001 app:app >/tmp/bare.log 2>&1) & PID=$!
    wait_up "$BASE"; t1=$(now_ms)
    sleep 1; IDLE=$(sum_rss_mib $PID)
    ab_run "$BASE"; LOADED=$(sum_rss_mib $PID)
    echo "bare,$r,$((t1-t0)),$IDLE,$LOADED,$DISK,$REQS,$CONC,$RPS,$MEAN,$P95,$FAIL" | tee -a "$CSV"
    kill "$PID"; wait "$PID" 2>/dev/null || true; sleep 1
  done ;;
container)
  BASE=http://127.0.0.1:8000
  DISK=$(docker image inspect simats/result-portal:1.0 --format '{{.Size}}' | awk '{printf "%.1f", $1/1048576}')
  mem() { docker stats --no-stream --format '{{.MemUsage}}' bench-portal | awk '{v=$1; if (v ~ /GiB/) {sub(/GiB/,"",v); v*=1024} else {sub(/MiB/,"",v)} print v}'; }
  for r in $(seq 1 "$RUNS"); do
    docker rm -f bench-portal >/dev/null 2>&1 || true
    t0=$(now_ms); docker run -d --name bench-portal -p 8000:8000 simats/result-portal:1.0 >/dev/null
    wait_up "$BASE"; t1=$(now_ms)
    sleep 1; IDLE=$(mem)
    ab_run "$BASE"; LOADED=$(mem)
    echo "container,$r,$((t1-t0)),$IDLE,$LOADED,$DISK,$REQS,$CONC,$RPS,$MEAN,$P95,$FAIL" | tee -a "$CSV"
    docker rm -f bench-portal >/dev/null
  done ;;
vm)
  : "${VM_URL:?give the VM url}"
  echo "Start-up: time VM boot + service start by hand (vm_deployment.md). Memory: 'free -m' in the guest."
  echo "Disk: size of the .vdi/.qcow2 on the host. Only rps/latency are measured by this script."
  for r in $(seq 1 "$RUNS"); do
    ab_run "$VM_URL"
    echo "vm,$r,MANUAL,MANUAL,MANUAL,MANUAL,$REQS,$CONC,$RPS,$MEAN,$P95,$FAIL" | tee -a "$CSV"
  done ;;
*) echo "unknown mode"; exit 2 ;;
esac
