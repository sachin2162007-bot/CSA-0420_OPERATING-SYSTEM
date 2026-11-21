# B3 – Compare Bare Metal, Virtual Machine and Container (8 marks)
S. Sachin, 192525045, CSA04. Evidence labels: ACTUAL / PENDING / ASSUMPTION (see `../README.md`).

## B3.1 Deploy three ways
The Part 3 Flask portal ran with the same gunicorn command (1) on Linux in a venv, (2) in a VM – **PENDING**, procedure in `Task01/vm_deployment.md`, (3) in a container built from the unmodified Part 3 Dockerfile (`whoami` = `portal`). Substitution: local stand-in base image because Docker Hub is blocked.

## B3.2 Measurements (ACTUAL, 1 vCPU / 3992 MiB / Ubuntu 24.04.5, `ab -n 5000 -c 50 /results`, 3 runs, 0 failures)
| | Bare metal | VM | Container |
|---|---|---|---|
| Start-up | 139 ms | PENDING | 318 ms |
| Memory idle/loaded | 55.1 / 55.2 MiB RSS | PENDING | 31.1 / 31.3 MiB cgroup |
| Disk / image | 36 MiB | PENDING | 62.3 MB content (243 MB on disk, stand-in base) |
| Requests/s | 5,190 | PENDING | 2,006 |
Caveats: memory metrics differ; client and server share one vCPU; the container goes via Docker NAT/proxy (`Task01/measurement_methodology.md`).

## B3.3 Kubernetes (ACTUAL, k3s single node)
Official `deployment.yaml` with TODO 1 (3 replicas, selector `app: result-portal`) and TODO 2 (requests 250m/256Mi, limits 500m/512Mi): 3/3 Running, `describe` shows the values. Deleting a pod produced a replacement Running in about 3 s. Scaling to 6 left 3 pods Pending (`Insufficient cpu`: 6 x 250m > 1 CPU); with a 100m request test variant 6/6 ran. Details: `Task02/`.

## B3.4 Trade-off
Bare metal is fastest and leanest; containers give reproducible images and fast start at some measured overhead in this setup; VMs give isolation at the cost of a guest OS (to be measured); Kubernetes adds self-healing and scaling but needs node capacity to match requests. Use rules and full explanation: `Task01/deployment_comparison.md`.

## Alternative: Compose vs Kubernetes
Compose restarts failed containers but cannot survive server loss and is bound to one machine; Kubernetes recreates pods (measured about 3 s), reschedules after node failure, and spans nodes. Table: `Task02/kubernetes_scaling.md`.
