# Part 4 – Deployment, Kubernetes and Decision (B3 and B4)

| | |
|---|---|
| Name / Register number | S. Sachin / 192525045 |
| Course | CSA04 Operating Systems, B.Tech AI and ML, SIMATS Engineering – Assignment 5 (CO5) |
| Allocation | B3 (8 marks) + B4 (5 marks) = 13 marks |
| Environment | Ubuntu 24.04.5 micro-VM, 1 vCPU, 3992 MiB, Docker 29.1.3, k3s v1.37.2 (single node) |

Evidence labels: ACTUAL = run in the environment above; PENDING = not run, procedure given; ASSUMPTION = stated input, not measured.

## TODO checklist
| Item | Status |
|---|---|
| deployment.yaml TODO 1 and TODO 2 | Done, applied, verified |
| B3.1 bare metal / container / VM | Done / Done / PENDING |
| B3.2 measurements | bare + container ACTUAL; VM PENDING |
| B3.3 Kubernetes (3 replicas, delete pod, scale to 6) | Done on k3s; scale-to-6 with official requests left 3 Pending (1-CPU node) |
| B3.4 trade-off | Done (VM column pending) |
| B4.1 second service | Done, both Running with own limits |
| B4.2 capacity | Done from measured 2,005.6 rps, assumptions stated |
| B4.3 decision | Done; cloud prices symbolic |

## Known deviations / pending
* k3s instead of minikube/kind (registries blocked); `python:3.12-slim` replaced by a local stand-in base, so image size differs from the real image.
* VM deployment not executed (no `/dev/kvm`). No screenshots (none fabricated); command outputs are in the `actual_output*.txt` files.
* `docker compose up --scale portal=6` not executed; comparison is analytical.
* Single vCPU shared by load generator and server; see `Task01/measurement_methodology.md`.

## Integration
Place this folder beside `PART3_LINUX_DOCKER_IMPLEMENTATION/`. Image tag `simats/result-portal:1.0` (built from Part 3 `Task02`) matches `Task02/deployment.yaml`.
Git: one commit per task (Task01 … Task04, REPORT).
