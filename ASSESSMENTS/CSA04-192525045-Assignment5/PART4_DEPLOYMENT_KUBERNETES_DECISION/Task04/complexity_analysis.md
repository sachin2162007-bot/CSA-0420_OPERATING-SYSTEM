# Complexity analysis (B3 and B4 alternative concepts)

Let n = replicas/instances, m = nodes, R = requests, c = per-instance capacity (measured rps).

| Topic | Time | Space | Note |
|---|---|---|---|
| Serving R requests with n identical instances | O(R / (n c)) wall time | O(n) instances x per-instance memory (measured 31 MiB container, 55 MiB bare RSS) | Linear speed-up only while n instances have separate cores; measured: 3 pods on 1 vCPU gave about 1.27x one container, not 3x |
| Capacity N = ceil(R_peak / (c u)) | O(1) arithmetic | O(1) | `capacity_estimation.md` |
| Kubernetes scheduling of n pods | O(n x m) filter/score per pod pass | O(n + m) cluster state | Pods start in parallel across nodes |
| Compose `--scale n` | O(n) sequential start on one host | O(n) on one host | Bound by one machine |
| Cloud fixed fleet vs autoscaled cost | O(N H p) vs O(n0 H p + 2 (N - n0) W p) | - | `cloud_cost_comparison.md` |
| Pod recovery | O(1) per pod (measured about 3 s) | - | Node failure adds detection delay |
