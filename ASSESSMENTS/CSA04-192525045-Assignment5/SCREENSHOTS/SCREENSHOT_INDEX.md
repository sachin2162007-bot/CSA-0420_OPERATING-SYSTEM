# Assignment 5: Comprehensive Screenshot and Visual Index

**Student Name**: S. Sachin  
**Register Number**: 192525045  
**Course Code**: CSA04 – Operating Systems (CO5)  
**Institution**: SIMATS Engineering  

---

## Visual Evidence Architecture

In accordance with academic integrity guidelines, all visual items are strictly categorized into:
1. **ILLUSTRATIVE VISUALS**: High-fidelity architectural diagrams and reference terminal visualisations generated to illustrate expected behavior and system states. These reside inside `ILLUSTRATIVE_VISUALS/` directories.
2. **ACTUAL MEASURED EVIDENCE**: Genuine terminal command logs, raw execution records, and text output files captured directly from Linux, Docker, and Kubernetes environments (e.g., `actual_output.txt`).

---

## Complete 34-Screenshot Mapping Table

| # | Filename | Subdirectory Location | Official Assignment Task | Concept / System State Illustrated | Type |
| :---: | :--- | :--- | :---: | :--- | :---: |
| 1 | `linux_system_information.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.1 | Kernel version, OS release (`uname -a`, `/etc/os-release`), architecture | Illustrative Visual |
| 2 | `linux_process_monitoring.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.1 | Process tree, PID status, CPU/thread state (`ps aux`, `top`) | Illustrative Visual |
| 3 | `linux_memory_monitoring.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.1 | Memory allocation, buffer/cache, swap space (`free -m`, `vmstat 1 5`) | Illustrative Visual |
| 4 | `linux_disk_monitoring.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.1 | Filesystem mount capacity, disk I/O throughput (`df -h`, `iostat -xz 1 3`) | Illustrative Visual |
| 5 | `linux_network_monitoring.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.1 | Listening TCP/UDP sockets, established flows (`ss -tulpn`) | Illustrative Visual |
| 6 | `log_analysis_execution.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.2 | Execution of `log_analysis.sh` with TODO 1 & 2 matching expected output | Illustrative Visual |
| 7 | `disk_space_alert_execution.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.2 | `disk_space_alert.sh` execution testing 85% threshold warning | Illustrative Visual |
| 8 | `cron_configuration.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.2 | Crontab hourly log analysis and 15-min disk alert scheduling (`crontab -l`) | Illustrative Visual |
| 9 | `systemd_service_status.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.3 | `systemctl status result-portal` showing active (running) systemd state | Illustrative Visual |
| 10 | `nginx_reverse_proxy.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.3 | Nginx reverse proxy routing port 80 -> 127.0.0.1:8000 | Illustrative Visual |
| 11 | `firewall_configuration.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.3 | UFW firewall status showing ports 22, 80, 443 allowed and all else blocked | Illustrative Visual |
| 12 | `portal_browser_test.png` | `PART3/B1_LINUX/ILLUSTRATIVE_VISUALS/` | Task B1.3 | Web browser rendering portal semester results table at `http://localhost/` | Illustrative Visual |
| 13 | `dockerfile_build.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker build -t result-portal .` executing layer cache and pip install | Illustrative Visual |
| 14 | `docker_image_list.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker images` showing slim image footprint (~175 MB) | Illustrative Visual |
| 15 | `docker_container_running.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker ps` displaying active `result-portal` container mapped to 8000 | Illustrative Visual |
| 16 | `portal_docker_browser.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | Browser testing portal served from Docker container at `localhost:8000` | Illustrative Visual |
| 17 | `docker_nonroot_user.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker exec <id> whoami` proving non-root user `portal` execution | Illustrative Visual |
| 18 | `docker_compose_services.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker compose ps` showing healthy `portal` web and `db` postgres containers | Illustrative Visual |
| 19 | `docker_volume_verification.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker volume inspect pgdata` demonstrating persistent database storage | Illustrative Visual |
| 20 | `docker_resource_limits.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker inspect` verifying cgroup `NanoCPUs` (0.5 CPU) and `Memory` (256MB) | Illustrative Visual |
| 21 | `docker_stats.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | Real-time `docker stats` streaming CPU %, Memory %, and I/O limits | Illustrative Visual |
| 22 | `docker_history.png` | `PART3/B2_DOCKER/ILLUSTRATIVE_VISUALS/` | Task B2 | `docker history result-portal` displaying layer breakdown and optimization | Illustrative Visual |
| 23 | `bare_metal_deployment.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.1 | Gunicorn running directly on Linux bare-metal host | Illustrative Visual |
| 24 | `virtual_machine_running.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.1 | Linux VM running portal guest under KVM / VirtualBox hypervisor | Illustrative Visual |
| 25 | `docker_deployment_running.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.1 | Containerized portal running under Docker engine | Illustrative Visual |
| 26 | `deployment_benchmark_results.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.2 | Benchmark throughput (RPS), startup latency, and memory comparison table | Illustrative Visual |
| 27 | `kubernetes_three_replicas.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.3 | `kubectl get pods` showing 3 replicas in `Running` state | Illustrative Visual |
| 28 | `kubernetes_pod_recovery.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.3 | ReplicaSet self-healing recreation after `kubectl delete pod` | Illustrative Visual |
| 29 | `kubernetes_six_replicas.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.3 | Horizontal scaling to 6 active replicas (`kubectl scale --replicas=6`) | Illustrative Visual |
| 30 | `kubernetes_resource_limits.png` | `PART4/B3_DEPLOYMENT/ILLUSTRATIVE_VISUALS/` | Task B3.3 | `kubectl describe pod` displaying requests (250m/256Mi) & limits (500m/512Mi) | Illustrative Visual |
| 31 | `second_service_running.png` | `PART4/B4_ENGINEERING_DECISION/ILLUSTRATIVE_VISUALS/` | Task B4.1 | Second service (`notice-service`) running concurrently alongside `result-portal` | Illustrative Visual |
| 32 | `both_services_resource_limits.png` | `PART4/B4_ENGINEERING_DECISION/ILLUSTRATIVE_VISUALS/` | Task B4.1 | Resource quota verification demonstrating independent limits on both services | Illustrative Visual |
| 33 | `capacity_estimation.png` | `PART4/B4_ENGINEERING_DECISION/ILLUSTRATIVE_VISUALS/` | Task B4.2 | Mathematical capacity sizing graph for 150,000 students in 2 hours | Illustrative Visual |
| 34 | `final_deployment_comparison.png` | `PART4/B4_ENGINEERING_DECISION/ILLUSTRATIVE_VISUALS/` | Task B4.3 | Architecture comparison matrix: On-Premise vs Cloud vs Hybrid decision | Illustrative Visual |

---

## Genuine Output Verification Files

Genuine execution evidence is captured in raw terminal output files:
- `PART3_LINUX_DOCKER_IMPLEMENTATION/Task01/actual_output.txt`: Raw server observation telemetry (`ps`, `free`, `vmstat`, `ss`, etc.).
- `PART3_LINUX_DOCKER_IMPLEMENTATION/Task02/actual_output.txt`: Genuine Docker build, compose startup, non-root user verification, and `docker stats`.
- `PART4_DEPLOYMENT_KUBERNETES_DECISION/Task02/actual_output.txt`: Genuine `kubectl get pods`, self-healing pod recovery, and 6-replica scaling log.
- `PART4_DEPLOYMENT_KUBERNETES_DECISION/Task03/actual_output.txt`: Multi-service scheduling, resource limit enforcement, and node CPU capacity allocation.
