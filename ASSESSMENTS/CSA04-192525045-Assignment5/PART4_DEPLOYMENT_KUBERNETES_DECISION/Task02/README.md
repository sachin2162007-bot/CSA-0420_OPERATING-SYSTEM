# B3.3 – Scale with Kubernetes

Cluster used (ACTUAL): **k3s v1.37.2, single node** (1 vCPU, 4 GB). minikube/kind need images from registries that are blocked in the
sandbox, so k3s was used; the `deployment.yaml` is standard Kubernetes and applies unchanged on minikube/kind
(`minikube image load simats/result-portal:1.0` or `kind load docker-image simats/result-portal:1.0` first).
Sandbox-only k3s settings: cgroup v1 allowed, containerd `restrict_oom_score_adj`, and relaxed disk-eviction thresholds (only ≈10 GB of 252 GB free, so the kubelet
otherwise taints the node with disk-pressure). None of these affect your minikube/kind run.

| Official TODO | Done as |
|---|---|
| TODO 1 | `replicas: 3` and `selector.matchLabels.app: result-portal` |
| TODO 2 | requests `cpu: 250m`, `memory: 256Mi`; limits `cpu: 500m`, `memory: 512Mi` |

Everything else is as in the PDF (image `simats/result-portal:1.0`, containerPort 8000, readinessProbe `/health:8000`, Service type LoadBalancer 80→8000).
`service.yaml` is an identical copy of the Service document already inside `deployment.yaml`.

Files: `scaling_experiment.sh` (run), `actual_output.txt` (ACTUAL output of the run), `supplementary_output*.txt`, `pod_recovery.md`, `kubernetes_scaling.md`, `expected_output.txt`.
Run: `bash scaling_experiment.sh` (`KUBECTL="k3s kubectl"` on k3s).
