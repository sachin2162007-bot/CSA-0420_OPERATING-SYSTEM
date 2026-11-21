# Task A3.2 – Benefits and Costs of Virtual Machines (2 Marks)

**Question (from the assignment):** *Evaluate the benefits of virtual machines for the university (isolation, consolidation, snapshots, live migration) and their costs (overhead, memory per VM, licences).*

## 1. Note on evidence

**No measurements were taken and no prices are stated.** Overheads are described qualitatively and with symbolic models. Actual numbers must come from the student's own tests in Part B (`free -m`, `vmstat`, a CPU benchmark and a disk benchmark run on bare metal and inside a VM). Licence terms must be taken from the vendor's current documentation at the time of costing.

## 2. Benefits for the university

### 2.1 Isolation
Each VM has its own guest kernel, memory and virtual devices. A crash, a kernel panic, a runaway process or a compromised application inside the `portal-web` VM does not directly affect the `portal-db` VM or other tenants such as the learning-management system. The boundary is the hypervisor, whose attack surface (a small, well-defined interface of hypercalls and device emulation) is much smaller than the whole Linux system-call interface that containers share (see A4). VM-escape vulnerabilities exist but are rare and are patched centrally.
*For the portal:* the database VM can sit in the internal subnet of Part 1 (10.20.20.0/24) and have different users, firewall rules and patch schedules from the web VM.

### 2.2 Server consolidation
Before virtualisation, one server often ran one service at low average utilisation. With VMs, several workloads share one physical host. Idle CPU and memory of one VM can be used by another; memory can be **over-committed** with techniques such as ballooning and page sharing (KSM) when the sum of the guests' peak needs rarely occurs together.
*For the portal:* a portal that has a few hundred users per day uses very little capacity for most of the year. The same host can also run the learning-management system and the examination system in separate VMs, which lowers the number of physical machines, power, cooling and rack space. Limitation: on result day, consolidated workloads compete for the same hardware, so capacity must be planned (Part B4).

### 2.3 Snapshots
A snapshot records a VM's disk (and optionally memory) state at a point in time, usually with copy-on-write: later writes go to a new file and the old blocks are preserved. It allows quick rollback before a risky upgrade, a clean test environment for the exam system, and a faster recovery. Costs: long snapshot chains slow disk access and use extra disk space; a snapshot is **not a backup** because it lives on the same storage (the backup plan in Part 1, A2.3, is still needed); and a memory snapshot taken while the database is running may capture inconsistent application state.

### 2.4 Live migration
Live migration moves a running VM from one physical host to another with a very short pause. The common method is **pre-copy**: memory pages are copied while the VM runs, pages that change ("dirty pages") are copied again in further rounds, then the VM is paused briefly to copy the last pages and device state (Clark et al., 2005). It allows host maintenance and kernel updates without taking the portal offline, and lets the university move VMs away from an overloaded host before result day. Requirements: two compatible hosts (similar CPU features), a fast network, and either shared storage or storage migration. Total migration time grows with VM memory size and with the rate at which memory is written; a write-heavy database may converge slowly. Live migration does **not** protect against a sudden host crash (that needs high availability or restart on another host).

## 3. Costs

### 3.1 CPU overhead
With VT-x/AMD-V, CPU-bound code runs almost directly on the processor, so the overhead is typically small. The overhead comes from **VM exits** (privileged events, interrupts), nested page-table walks (a TLB miss costs more with EPT/NPT), and scheduling of virtual CPUs by the hypervisor. I/O-heavy work has a larger overhead than CPU-bound work, particularly with emulated devices; virtio and device assignment reduce it. Model: `T_vm = T_native + (number of VM exits) × (cost per exit) + I/O emulation time`. The size of the overhead for the portal is a Part B measurement.

### 3.2 Memory used per VM
Each VM needs memory for the **guest kernel**, the guest's background services and page cache, **the application itself**, and a small amount for hypervisor structures (for KVM, the QEMU process and page tables). If `M_guest` is the guest OS base memory, `M_app` the application memory and `M_vmm` the per-VM hypervisor overhead, then for V VMs:

`M_total = M_host + V · (M_guest + M_app + M_vmm)`

so memory grows **O(V)** and the guest operating system part is duplicated in every VM. Techniques that reduce it: ballooning (the hypervisor reclaims unused guest memory), KSM (merge identical pages), minimal guest installs, and not over-committing database VMs (a database needs its working set in RAM).

### 3.3 Licensing
| Option | Licence position (verify current terms) |
|---|---|
| KVM + QEMU | Open source; no per-VM or per-host hypervisor fee. The cost is staff time and any commercial support contract the university chooses. |
| Xen | Open source hypervisor; commercial distributions may be paid. |
| VMware ESXi / vSphere | Commercial; the licensing model has changed since the Broadcom acquisition (subscription bundles, possible minimum quantities). Must be checked and costed from the current price list. |
| Hyper-V | Included with Windows Server licences; Windows guest VMs need their own Windows licences, which scale with host cores and the number of VMs (per Microsoft's licensing rules). |
| Guest operating systems | Linux guests: no per-VM licence fee for most distributions; Windows guests: licensed. |
| VirtualBox / VMware Workstation | Desktop products; terms for institutional or commercial use must be checked. |

Licence cost is therefore **O(hosts)** or **O(cores)** for commercial hypervisors and effectively **O(1)** for KVM (no per-host fee). No price is given here because current prices must come from the vendors.

### 3.4 Other costs
Operational complexity (a management layer, patching hypervisors, storage design), a single host failure affects every VM on it (mitigated by high availability and spreading VMs across hosts), and slower start (boot of a full guest OS, see A4).

## 4. Summary table

| Aspect | Benefit or cost | Effect on the portal |
|---|---|---|
| Isolation | Benefit: separate kernels | Web, database and other university systems cannot crash each other |
| Consolidation | Benefit | Fewer servers for the quiet 364 days; capacity must still be planned for result day |
| Snapshots | Benefit, with limits | Safe upgrades; not a substitute for backups |
| Live migration | Benefit | Maintenance without downtime |
| CPU overhead | Cost: usually small | Verify by measurement (Part B) |
| Memory per VM | Cost: O(V), duplicated guest OS | Limits how many portal instances one host can carry |
| Licences | Cost: none for KVM, significant for some commercial products | Favours KVM for a Linux-skilled team |

## 5. Evaluation

For the portal, VMs give strong isolation and good consolidation at a modest CPU cost; the main costs are memory per instance, slow scaling (a new VM must boot a whole guest OS) and, for commercial products, licensing. This supports using VMs as the **infrastructure boundary** (for example the database host and the Kubernetes worker nodes) and examining containers for the **portal application tier**, which A4 and A5 develop.
