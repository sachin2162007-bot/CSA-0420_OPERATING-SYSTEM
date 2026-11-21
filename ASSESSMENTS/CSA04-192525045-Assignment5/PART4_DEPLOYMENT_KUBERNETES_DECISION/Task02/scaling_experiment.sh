#!/usr/bin/env bash
# B3.3: apply the official deployment.yaml, verify, kill a pod, scale to 6 replicas under load.
# Works on minikube / kind (KUBECTL=kubectl) or on k3s (KUBECTL="k3s kubectl", the cluster used for the recorded run).
# The image simats/result-portal:1.0 must already be inside the cluster:
#   minikube image load simats/result-portal:1.0     |   kind load docker-image simats/result-portal:1.0
set -uo pipefail
HERE=$(cd "$(dirname "$0")" && pwd)
KUBECTL=${KUBECTL:-kubectl}
LB=${LB:-}            # URL for load; default = port-forward of the Service on :8080
step() { printf '\n===== %s =====\n' "$*"; }

step "kubectl apply -f deployment.yaml"
$KUBECTL apply -f "$HERE/deployment.yaml"
$KUBECTL rollout status deployment/result-portal --timeout=180s

step "kubectl get pods / deployments / services"
$KUBECTL get pods -o wide
$KUBECTL get deployments
$KUBECTL get services

step "kubectl describe pod (requests and limits)"
POD=$($KUBECTL get pods -l app=result-portal -o jsonpath='{.items[0].metadata.name}')
$KUBECTL describe pod "$POD" | sed -n '/^Name:/p;/^Labels:/p;/^Status:/p;/Limits:/,/Readiness:/p;/QoS Class:/p'

step "Delete one pod; Kubernetes must recreate it"
date +%T.%N
$KUBECTL delete pod "$POD" --wait=false
for i in 1 2 3 4 5 6 7 8 9 10; do
  $KUBECTL get pods -l app=result-portal --no-headers; echo "--- t+${i}s"; sleep 1
done
$KUBECTL get pods -l app=result-portal

step "Scale to 6 replicas while 20 s of load is running"
if [ -z "$LB" ]; then
  $KUBECTL port-forward service/result-portal 8080:80 >/tmp/pf.log 2>&1 & PF=$!
  sleep 2; LB=http://127.0.0.1:8080
fi
( ab -t 20 -c 50 "$LB/results" > /tmp/ab_scale_load.txt 2>&1 ) & ABPID=$!
sleep 3
date +%T; $KUBECTL scale deployment/result-portal --replicas=6
for i in 1 2 3 4 5 6; do $KUBECTL get deployment result-portal --no-headers; sleep 3; done
wait "$ABPID"
grep -E "Requests per second|Failed requests|Complete requests" /tmp/ab_scale_load.txt
[ -n "${PF:-}" ] && kill "$PF" 2>/dev/null

step "State after scaling to 6"
$KUBECTL get pods -o wide
$KUBECTL describe nodes | sed -n '/Allocated resources:/,/Events:/p'
PENDING=$($KUBECTL get pods -l app=result-portal --field-selector=status.phase=Pending -o name | head -1)
[ -n "$PENDING" ] && $KUBECTL describe "$PENDING" | sed -n '/^Events:/,$p'
