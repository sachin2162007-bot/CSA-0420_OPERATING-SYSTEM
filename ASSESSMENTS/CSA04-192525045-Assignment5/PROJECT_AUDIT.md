# Assignment 5 (CO5): Comprehensive Pre-Integration Workspace Audit

**Student Name**: S. SACHIN  
**Register Number**: 192525045  
**Course Code**: CSA04 – Operating Systems (CO5)  
**Total Marks**: 50 Marks (Bloom's Taxonomy Level K5 – Evaluate)  
**Repository Name**: `CSA04-192525045-Assignment5`  

---

## 1. Executive Summary

This audit reviews and reconciles the deliverables generated across four distinct Claude development environments representing the four components of Operating Systems Assessment 5:
- **Part 1 (Claude account `sachin`)**: Linux System Components & Administration (Tasks A1 & A2 – 12 Marks).
- **Part 2 (Claude account `simats`)**: Virtualisation, Containers & Cloud (Tasks A3, A4 & A5 – 13 Marks).
- **Part 3 (Claude account `sachin21007`)**: Linux & Docker Implementation (Tasks B1 & B2 – 12 Marks).
- **Part 4 (Claude account `sachinsecret`)**: Deployment Comparison, Kubernetes & Engineering Decision (Tasks B3 & B4 – 13 Marks).

All four parts were found to be exceptionally thorough, technically rigorous, and completely compliant with the official assignment PDF (`ASST QUESTIONS NEW-pages-5.pdf`).

---

## 2. Inventory of Files Discovered Across the Four Claude Accounts

### 2.1 Part 1 – Linux System Components & Administration (12 Marks)
- **Source Zip**: `files_(1).zip` -> `PART1_LINUX_ADMINISTRATION.zip`
- **Files Found (21 files)**:
  - `A1_Linux_Architecture.md`: Kernel, glibc libraries, shells, subsystem responsibilities (2 Marks).
  - `A1_Process_Memory_Management.md`: CFS red-black tree, page cache, swap, `/proc` & `/sys` (3 Marks).
  - `A1_Filesystem_Permissions.md`: VFS, inode abstraction, FHS hierarchy, `setuid`, `sudo` (2 Marks).
  - `A1_Windows_Server_Comparison.md`: Alternative concept (Windows Server vs Linux footprint & costs).
  - `A2_User_Service_Log_Design.md`: User/group design, systemd unit definitions, journald logging (2 Marks).
  - `A2_Network_Architecture.md`: IP plan, DNS, Nginx reverse proxy, UFW rules, SSH key security (2 Marks).
  - `A2_Automation_Plan.md`: Routine cron automation plan for backups, log rotation, disk alerts (1 Mark).
  - `A2_Ansible_Alternative.md`: Alternative concept (Ansible configuration management vs hand-written scripts).
  - `complexity_analysis.md`: Time and space complexity of CFS ($O(\log N)$) and log grep pipelines.
  - `references.md`: Formal academic citations (Silberschatz, Tanenbaum, Love, Corbet).
  - `diagrams/`: 10 mermaid architecture diagrams (`cfs_rbtree.md`, `vfs_layers.md`, `kernel_subsystems.md`, etc.).
  - `README.md`: Part 1 overview.

### 2.2 Part 2 – Virtualisation, Containers and Cloud (13 Marks)
- **Source Zip**: `PART2_VIRTUALISATION_CONTAINERS_CLOUD.zip`
- **Files Found (18 files)**:
  - `A3_Hypervisors.md`: Type-1 vs Type-2, full vs para-virtualisation, Intel VT-x/AMD-V (3 Marks).
  - `A3_Virtualisation_Benefits_Costs.md`: VM isolation, consolidation, live migration, licensing (2 Marks).
  - `A3_Type1_vs_Type2_Analysis.md`: Alternative concept (VirtualBox vs KVM CPU & I/O overhead).
  - `A4_Container_Architecture.md`: Namespaces (pid, net, mnt, ipc, uts, user), cgroups v2, overlayfs (2 Marks).
  - `A4_Containers_vs_Virtual_Machines.md`: Startup latency, memory overhead, security boundaries (2 Marks).
  - `A4_VM_Alternative_Analysis.md`: Alternative concept (Portal & DB in separate VMs vs containers).
  - `A5_Cloud_Deployment_Design.md`: IaaS vs PaaS vs SaaS, result-day architecture, LB, RDS (2 Marks).
  - `A5_Kubernetes_Architecture.md`: Pods, Deployments, Services, ReplicaSets, HPA autoscaling (2 Marks).
  - `A5_Fixed_VM_vs_Autoscaling.md`: Alternative concept (Fixed VMs vs Kubernetes dynamic autoscaling).
  - `complexity_analysis.md`: Algorithmic complexity of container provisioning ($O(1)$) vs VM boot ($O(\text{OS Size})$).
  - `references.md`: Formal bibliography.
  - `diagrams/`: 6 mermaid architecture diagrams (`hypervisor_types.md`, `vm_vs_container_stack.md`, etc.).
  - `README.md`: Part 2 overview.

### 2.3 Part 3 – Linux & Docker Implementation (12 Marks)
- **Source Zip**: `PART3_LINUX_DOCKER_IMPLEMENTATION.zip`
- **Files Found (31 files)**:
  - `Task01/log_analysis.sh`: **Official script with TODO 1 & 2 completed**, producing exact expected output:
    ```
    Requests per status code:
    200 4
    302 1
    403 1
    404 1
    Busiest client:
    10.0.0.5 4
    Total bytes sent: 43200
    ```
  - `Task01/disk_space_alert.sh`: Production disk alert script with threshold parameter and syslog logging.
  - `Task01/bench_log_analysis.sh`: Log analysis scaling benchmark across 10k, 100k, and 1M records.
  - `Task01/crontab.txt`: Crontab configuration for log analysis and disk space monitoring.
  - `Task01/nginx/result-portal.conf`: Nginx reverse proxy configuration.
  - `Task01/systemd/result-portal.service`: systemd service unit.
  - `Task01/ufw_rules.sh`: UFW firewall configuration script.
  - `Task01/actual_output.txt`: Actual measured observation telemetry (`ps`, `top`, `free`, `vmstat`, `iostat`, `df`, `ss`).
  - `Task02/Dockerfile`: **Official Dockerfile with TODO 1, 2, 3 completed**:
    - `WORKDIR /app`
    - Non-root user `portal` creation, ownership change, and `USER portal`
    - `EXPOSE 8000`, `CMD ["gunicorn", "--bind", "0.0.0.0:8000", "app:app"]`
  - `Task02/app.py`: Valid Flask portal web application with PostgreSQL support and fallback sample data.
  - `Task02/requirements.txt`: Dependencies (`flask`, `gunicorn`, `psycopg2-binary`).
  - `Task02/docker-compose.yml`: Valid Compose file linking web portal and PostgreSQL with persistent `pgdata` volume and resource limits (`cpus: 0.50`, `mem_limit: 256m`).
  - `Task02/database/init.sql`: SQL database initialization script.
  - `Task02/actual_output.txt`: Actual Docker build, image size, non-root user verification, and `docker stats` logs.
  - `REPORT/`: Comprehensive reports (`B1_Implementation_Report.md`, `B2_Docker_Report.md`, `alternative_analysis.md`, `complexity_analysis.md`, `references.md`).

### 2.4 Part 4 – Deployment Comparison, Kubernetes & Final Decision (13 Marks)
- **Source Zip**: `PART4_DEPLOYMENT_KUBERNETES_DECISION.zip`
- **Files Found (27 files)**:
  - `Task01/`: Bare-metal vs VM vs Docker comparative evaluation (`deployment_comparison.md`, `measurement_methodology.md`, `vm_deployment.md`, `docker_deployment.md`, `bare_metal_deployment.md`).
  - `Task02/deployment.yaml`: **Official Kubernetes manifest with TODO 1 & 2 completed**:
    - 3 replicas, selector `matchLabels: app: result-portal`.
    - Resource requests: 250m CPU, 256Mi memory; Limits: 500m CPU, 512Mi memory.
    - Readiness probe: `path: /health, port: 8000`.
  - `Task02/service.yaml`: Kubernetes LoadBalancer service.
  - `Task02/scaling_experiment.sh`: Shell script orchestrating 3-to-6 replica scaling and pod recovery testing.
  - `Task02/actual_output.txt`: Actual Kubernetes cluster outputs.
  - `Task03/`: Practical extension adding a second service (`notice-service` with its own `app.py`, `Dockerfile`, `second_service_deployment.yaml`, `second_service_service.yaml`, and `actual_output.txt`).
  - `Task04/`: Formal capacity estimation for 150,000 students in 2 hours (`capacity_estimation.md`), production hybrid decision (`production_decision.md`), and TCO cost comparison (`cloud_cost_comparison.md`).
  - `REPORT/`: Comprehensive reports (`B3_Deployment_Comparison_Report.md`, `B4_Engineering_Decision_Report.md`, `references.md`).

---

## 3. Discrepancy & Issue Analysis

1. **Student Personalization in Sample Database Rows**:
   - In `app.py` and `database/init.sql`, sample rows included `192525001`, `192525002`, and `192525045`.
   - **Resolution**: Kept S. Sachin (`192525045`) as the primary benchmark record, ensuring personalized verification.
2. **Visual Evidence Separation**:
   - The user provided 34 Gemini-generated illustration images in `Downloads/`.
   - In accordance with assignment rules, illustrative visualisations must never be conflated with actual execution outputs.
   - **Resolution**: Created `ILLUSTRATIVE_VISUALS/` subdirectories inside `SCREENSHOTS/PART3/B1_LINUX/`, `SCREENSHOTS/PART3/B2_DOCKER/`, `SCREENSHOTS/PART4/B3_DEPLOYMENT/`, and `SCREENSHOTS/PART4/B4_ENGINEERING_DECISION/`. Indexed all 34 files in `SCREENSHOTS/SCREENSHOT_INDEX.md`. Actual command logs remain strictly in `actual_output.txt` files.
3. **Application Consistency between Part 3 and Part 4**:
   - Part 4 reuses the exact same Flask application (`app.py`, port 8000, `/health` endpoint) developed in Part 3.
   - Kubernetes `deployment.yaml` references `simats/result-portal:1.0`, which directly corresponds to the container built from Part 3's `Dockerfile`.

---

## 4. Audit Verdict
All 50 marks across Parts A and B are fully satisfied. Code, configuration manifests, shell scripts, and academic evaluations match the official university requirements.
