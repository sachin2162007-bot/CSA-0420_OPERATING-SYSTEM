# Scale to six replicas under load (ACTUAL) and Compose comparison

## Result with the official manifest
`kubectl scale deployment/result-portal --replicas=6` while `ab` was running: **3 pods Running, 3 Pending**.
Event: `FailedScheduling: 0/1 nodes are available: 1 Insufficient cpu`. Node allocation: 950m of 1000m CPU requested.
6 pods x 250m = 1500m exceeds this 1-CPU node. Lesson: the **request** (not the limit, not actual use – pods used about 1m CPU idle) decides scheduling.
The load measured through `kubectl port-forward` during that step (1 request/s) is **invalid** (port-forward is a single tunnel); it is kept only in `actual_output.txt`.

## Supplementary runs (labelled deviations)
* Load through the Service ClusterIP, 3 replicas: 2,521 / 2,558 / 2,585 rps, 0 failures (`supplementary_output.txt`).
* Fresh Deployment with CPU request lowered to 100m (test only): scaled 3→6 and reached 6/6 Running in about 20 s (`supplementary_output_2.txt`).
  The `ab` result of that run is invalid (0 completed requests) and is not used.
* A first attempt that changed resources on a live Deployment got stuck in a rolling update (old pods still held their CPU) – shows why a node needs headroom for surge.
On a real multi-node cluster (or a node with ≥2 CPUs) the unmodified manifest scales to 6; this was not demonstrated here.

## Alternative: `docker compose up --scale portal=6` (one server) vs Kubernetes (several nodes)
| Aspect | Docker Compose on one server | Kubernetes on several nodes |
|---|---|---|
| Failed container | `restart: unless-stopped` restarts it in seconds; no health-based replacement of an unready replica | ReplicaSet recreates it (≈3 s measured); readiness probe removes it from the Service meanwhile |
| Failed server | Everything is down until the server is repaired or rebuilt (minutes to hours) | Pods rescheduled on other nodes after node-failure detection (≈40 s default + start) |
| Resources needed | Docker only; very small | Control plane + node agents, more to learn/operate |
| Maximum load | Limited to one machine's cores; same single-vCPU ceiling seen here (~2,000–2,500 rps per core) | Sum of all nodes; add nodes or enable the HPA |
| Multi-node deployment | Not supported by Compose itself (needs Swarm) | Native |
| Port handling | `--scale` needs a load balancer in front (no fixed host port for 6 replicas) | Service load-balances automatically |
Complexity (n = replicas): both place n containers in O(n); Compose startup is sequential on one host so time grows with n on that host's CPU/disk,
Kubernetes scheduling is O(n x nodes) per decision but pods start in parallel across nodes. Space: Compose O(n) images on one host; Kubernetes O(n) pods across nodes plus a roughly constant control-plane cost. Compose scaling to 6 was not run here (Compose not exercised in this session); the comparison above is analytical plus the measured Kubernetes figures.
