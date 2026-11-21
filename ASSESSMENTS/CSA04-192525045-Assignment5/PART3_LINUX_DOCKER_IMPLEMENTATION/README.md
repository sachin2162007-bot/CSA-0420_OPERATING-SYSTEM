# Part 3 – Linux Administration and Containerisation (B1 and B2)

| | |
|---|---|
| Name | S. Sachin |
| Register number | 192525045 |
| Course | CSA04 Operating Systems, B.Tech AI and ML, SIMATS Engineering |
| Assignment | Assignment 5 – CO5 |
| Allocation | B1 (7 marks) + B2 (5 marks) = 12 marks. B3 and B4 are not included |
| Environment | Ubuntu 24.04.5 micro-VM, 1 vCPU, 3992 MiB, bash 5.2.21, no systemd as PID 1, Docker Hub unreachable |

Evidence labels: **ACTUAL** (run here), **PENDING** (not run, command given), **EXPECTED/EXAMPLE** (from the PDF or reasoning).

## Contents
- `Task01/` – B1: scripts, cron, observation, systemd, Nginx, firewall (README inside)
- `Task02/` – B2: Dockerfile, Compose, app, docs, `capture_evidence.sh` (README inside)
- `REPORT/` – B1 and B2 reports, alternative analysis, complexity analysis, references

## TODO checklist
| Official TODO | Status |
|---|---|
| B1 log_analysis TODO 1 (status codes) | Done, output matches expected |
| B1 log_analysis TODO 2 (busiest client) | Done, output matches expected |
| B2 Dockerfile TODO 1 `WORKDIR /app` | Done (build PENDING) |
| B2 Dockerfile TODO 2 non-root `portal` | Done (build PENDING) |
| B2 Dockerfile TODO 3 `EXPOSE 8000` + Gunicorn | Done (build PENDING) |

## Pending
Docker build/run/stats/history/Compose evidence (`bash Task02/capture_evidence.sh`); systemd start and restart-on-kill; real hourly cron firing; HTTPS; screenshots.

## Part 4 integration
Image tag `simats/result-portal:1.0` for B3 `deployment.yaml`; port 8000 and `/health` match its probe; reuse `ab` for B3.2 and B4.2; B3/B4 use their own folders.

## Git commits (one per task)
`B1.1`, `B1.2 log_analysis`, `B1.2 disk alert + cron`, `B1.3 services`, `B1.4`, `B2 Dockerfile`, `B2 compose`, `B2 optimisation`, `reports`.
