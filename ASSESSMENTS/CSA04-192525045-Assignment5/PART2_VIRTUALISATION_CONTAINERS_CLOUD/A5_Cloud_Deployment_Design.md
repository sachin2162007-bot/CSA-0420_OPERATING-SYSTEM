# Task A5.1 – Cloud Set-up (2 Marks)

**Question (from the assignment):** *Compare IaaS, PaaS and SaaS, and design a cloud set-up for result day (virtual machines or a managed Kubernetes service, a load balancer, a managed database).*

Case study: the University Result Portal. See [Diagram D14](diagrams/cloud_deployment.md).

## 1. Note on evidence

**No prices, instance sizes or benchmark results are invented.** Sizes are expressed as formulas that are completed with the throughput measured in Part B (B3.2 and B4.2). Product names are given only as examples of a category, and any provider can be substituted. Terminology and addresses follow Part 1 (`portal-web`, `portal-db`, `results.example.edu`).

## 2. IaaS, PaaS and SaaS

The National Institute of Standards and Technology (NIST SP 800-145) defines three service models, which differ in **how much of the stack the provider manages**.

| Layer | On-premises (today) | IaaS | PaaS | SaaS |
|---|---|---|---|---|
| Application and data | University | University | University (application code and data) | Provider |
| Runtime, middleware | University | University | Provider | Provider |
| Operating system | University | University | Provider | Provider |
| Virtualisation, servers, storage, network | University | Provider | Provider | Provider |
| Physical data centre | University | Provider | Provider | Provider |

| Model | What the customer receives | Examples (category only) | Control | Effort | Fit for the portal |
|---|---|---|---|---|---|
| **IaaS** | Virtual machines, block storage, virtual networks, load balancers, rented per hour | Cloud virtual machines (such as Amazon EC2, Azure Virtual Machines, Google Compute Engine) | Highest: OS and software chosen by the university | Highest: patching, scaling, availability | Possible: the Part 1 design could be rebuilt on cloud VMs |
| **PaaS** | A managed platform to run code or containers or a managed service (database, Kubernetes control plane, application platform) | Managed Kubernetes (EKS, AKS, GKE), managed PostgreSQL, app platforms | Medium: application and configuration | Medium: provider runs the platform | **Recommended** for the scalable web tier and database |
| **SaaS** | A finished application used through a browser | Email suites, learning-management products | Lowest: configuration only | Lowest | Only if the university buys a ready-made results product; it removes engineering but also customisation and control of student data |

The managed Kubernetes control plane and the managed database are normally classed as **PaaS-like managed services**; the worker nodes of a managed cluster are IaaS virtual machines that the customer pays for and sizes. The responsibility split (shared-responsibility model) means the provider secures and runs what it manages, while the university remains responsible for application security, access control and its data in all three models.

## 3. Design goals for result day

From the case study: 150,000 students in two hours is about 150,000 ÷ 7,200 s ≈ **20.8 logins per second on average**. The real peak is higher, because students tend to arrive just after publication, and each login produces several HTTP requests. The design must therefore (1) absorb a sharp peak, (2) survive failure of a container, a node or a data-centre zone, (3) protect the database, and (4) avoid paying for result-day capacity for the whole year.

## 4. Proposed design

Choice between the two options in the assignment: **a managed Kubernetes service**, not self-managed VMs. Reasons: the provider operates the control plane, the portal is stateless so containers fit (A4.2), replicas and autoscaling are built in (A5.2), and capacity can be added without rebuilding servers. The VM option is kept as the alternative in [A5_Fixed_VM_vs_Autoscaling.md](A5_Fixed_VM_vs_Autoscaling.md).

| Component | Design | Purpose |
|---|---|---|
| **DNS** | Public zone with `results.example.edu` pointing to the load balancer; low TTL before result day (as in Part 1, A2.2) | Name students use; quick redirection if needed |
| **Load balancer** | Provider-managed, HTTP(S) (layer-7) balancer in front of the cluster, spread over **two or three availability zones**, TLS certificate at the balancer, health checks against `/health` | Single public entry point; distributes load; removes unhealthy targets; hides the nodes |
| **Managed Kubernetes cluster** | Worker nodes (VMs) in private subnets across the same zones; portal Deployment with replicas and an autoscaler (A5.2); a node autoscaler adds worker VMs when pods do not fit | Runs the portal containers and scales them |
| **Container registry** | Private registry holding the portal image | Nodes pull the identical image |
| **Managed database** | Managed PostgreSQL with a primary and a standby in another zone (automatic failover), automated backups and point-in-time recovery; in a private subnet, reachable only from the cluster nodes; an optional read replica for the read-heavy result lookups | Replaces the self-managed `portal-db` VM of Part 1 and keeps the data persistent and recoverable |
| **Connection pooling** | A pooler (in the application or a small proxy) | Prevents many replicas from exceeding the database connection limit |
| **Monitoring** | Cluster metrics (needed by the autoscaler) and provider metrics for the load balancer and database | Observe CPU, memory, latency and errors on result day |

**Not included, on purpose:** a content delivery network, a message queue, a separate cache cluster and other extra services (they would expand the scope and cost; they are possible later improvements if measurements show the need).

### 4.1 Request path

Student → DNS → load balancer (TLS, health checks) → Kubernetes Service → one of the portal pods (Nginx/Gunicorn container) → connection pool → managed PostgreSQL primary (or read replica). This is the cloud equivalent of the request flow in Part 1 (Diagram D8).

### 4.2 Network and security

- Only the load balancer has a public address; nodes and the database are in **private subnets**, matching the DMZ/internal separation of Part 1 (D7).
- Security groups / firewall rules: internet to load balancer on 443 (80 redirects to 443); load balancer to nodes on the service port; nodes to database on 5432 only; administrators through a controlled path (key-based, as in Part 1).
- Secrets (database credentials) are stored in the platform's secret store, not in the image; containers run as a non-root user (A4.2).

### 4.3 Sizing method (to be completed from measurements)

Let `μ` be the sustainable requests per second of one portal pod (measured in Part B), `k` the HTTP requests per login, `λ_login_peak` the peak logins per second, `ρ_target` the target utilisation (for example 0.6–0.7 to keep headroom):

`R_pods = ceil( λ_login_peak · k / (μ · ρ_target) )`

`N_nodes = ceil( R_pods · (CPU request per pod) / (allocatable CPU per node) )` (and the same check for memory)

The database connection budget must satisfy `R_pods · workers_per_pod · pool_per_worker ≤ max_connections`. The database, which does not scale by adding pods, is usually the **first bottleneck**, so the plan is to load-test it in Part B and to size the primary (and read replica) separately.

### 4.4 Result-day schedule

| When | Action | Reason |
|---|---|---|
| Weeks before | Lower DNS TTL; load-test; confirm provider quotas for CPU and load balancers | Quotas can block scaling |
| Day before | Raise the **minimum** number of replicas and nodes to the planned level (pre-warm); verify database size and connections | Autoscaling reacts after load arrives; new nodes take time to start |
| Result day | Watch latency, error rate, CPU and database connections; autoscaler adds replicas above the minimum | Absorb the peak |
| After the peak | Allow scale-down (the autoscaler removes pods slowly); lower the minimum to the everyday value | Stop paying for unused capacity |

## 5. Evaluation of the design

| Requirement | How the design meets it | Residual risk |
|---|---|---|
| Handle the peak | Many replicas behind a balancer; autoscaling and pre-warming | Autoscaler reaction time; provider quotas; database bottleneck |
| Recover from failure | Replicas across zones; health checks; database standby with failover | Failover takes some time; application must reconnect |
| Efficient use of resources | Small baseline, capacity added only when needed | Over-provisioning if the minimum is set too high |
| Control of student data | Database in the university's cloud account, private subnet, backups, encryption options | Data resides with a third-party provider (a policy question for Part B4) |
| Skills | Containers and Kubernetes knowledge needed | Training effort |

The use of the university's own server room (hybrid options) is not part of A5 and is decided in Part B4.
