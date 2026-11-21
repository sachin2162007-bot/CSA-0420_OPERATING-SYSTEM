# Task A4.2 – Containers versus Virtual Machines (2 Marks)

**Question (from the assignment):** *Compare containers and virtual machines for start-up time, memory use, isolation and security, and decide where each should be used for the portal.*

## 1. Note on evidence

Start-up and memory figures below are **qualitative orders of magnitude** based on the architecture, as the assignment itself states (seconds versus minutes). **No measurement is claimed**; the values are measured in Part B3.2. See [Diagram D12](diagrams/vm_vs_container_stack.md).

## 2. Architectural difference

| | Virtual machine | Container |
|---|---|---|
| What is virtualised | The **hardware** (CPU, memory, devices) | The **operating system's view** (namespaces) and resources (cgroups) |
| Kernel | Each VM has its **own guest kernel** | All containers **share the host kernel** |
| Image contents | Complete OS plus application, disk image of many GB | Application, libraries and a minimal user space; layers shared |
| Guest OS can differ from host | Yes (Windows guest on Linux host) | No: same kernel family (Linux containers need a Linux kernel) |

## 3. Comparison

| Criterion | Virtual machine | Container | Evaluation |
|---|---|---|---|
| **Start-up time** | Firmware start, guest kernel boot, init system, services: typically tens of seconds to minutes | Create namespaces and cgroups, mount layers, start one process: typically about a second or less once the image is on the node | Containers start much faster, which matters when scaling during the two-hour result window |
| **Memory use** | Guest kernel, OS services, page cache and hypervisor overhead per instance: `V·(M_guest + M_app + M_vmm)` | Application plus a small runtime overhead; kernel shared: `M_kernel + C·M_app` | Containers allow far more instances per host |
| **Disk use** | Full disk image per VM (copy-on-write clones help) | Layers shared between containers from the same image | Containers are smaller and faster to distribute |
| **Isolation** | Hardware-level boundary enforced by the hypervisor; separate kernels | Process-level boundary enforced by the shared kernel (namespaces, cgroups) | VMs give stronger isolation |
| **Security** | Small attack surface (hypervisor interface); a guest kernel exploit stays inside the guest | Large attack surface (the whole system-call interface of the shared kernel); a kernel vulnerability or container escape can expose the host and all other containers | VMs are safer for untrusted or mixed-trust workloads |
| **Performance** | Small CPU overhead; larger I/O overhead unless paravirtual drivers are used | Near-native CPU and, with volumes, near-native I/O; the overlay file system adds some overhead | Containers have lower overhead |
| **Density / cost** | Fewer instances per host | Many more instances per host | Containers reduce cost for stateless tiers |
| **Portability** | VM image tied to hypervisor format | Same image runs on any host with a compatible container runtime and kernel | Containers are easier to move and replicate |

## 4. Security measures that narrow the gap

Containers are hardened, not left at defaults: run as a **non-root user** (and use the user namespace for rootless operation), **drop Linux capabilities** that are not needed, apply a **seccomp** profile and AppArmor/SELinux policy, use a **read-only root file system**, set resource limits (cgroups), and keep images small and scanned for vulnerabilities (NIST SP 800-190). For higher assurance, **lightweight-VM runtimes** (for example Kata Containers) or user-space kernels (gVisor) place each container or pod behind a stronger boundary, at some cost in start-up time and overhead.

## 5. Decision for the university result portal

| Component | Recommended unit | Reason |
|---|---|---|
| Portal web application (Python/Gunicorn behind Nginx) | **Containers** | Stateless, identical replicas, must scale quickly on result day, and are rebuilt from one image; same image for development, testing and production |
| Reverse proxy / ingress | **Containers** | Same reasons; configuration is data |
| Database (PostgreSQL) | **VM, or a managed database service** (A5); a container only if persistent volumes, backups and recovery are well engineered | Stateful, I/O-sensitive, needs strong isolation, stable storage and careful memory tuning; it is not scaled by simply adding replicas |
| Container hosts (cluster nodes) | **Virtual machines** (cloud VMs or KVM guests on the university's servers) | A VM boundary separates the university from other tenants and from other university systems, and cloud providers supply nodes as VMs |
| Other university systems (learning-management, examination) | VMs or separate namespaces/node pools, depending on trust and licensing | Different owners and patch cycles; strong separation preferred |

**Overall:** use **VMs as the infrastructure and security boundary, and containers as the unit of application deployment and scaling**. The two are complementary rather than alternatives: the portal containers run inside VM-based worker nodes.

## 6. Evaluation

Containers answer the question "how do we start many identical copies in seconds?", which is the result-day problem. VMs answer "how do we isolate and manage the infrastructure strongly?" The reasoned combination is better than choosing only one. The cost of the shared kernel is accepted for the stateless tier because the data is protected separately (database in its own boundary, network policies and non-root, minimal containers). The consequences of using VMs for both tiers are analysed in [A4_VM_Alternative_Analysis.md](A4_VM_Alternative_Analysis.md).
