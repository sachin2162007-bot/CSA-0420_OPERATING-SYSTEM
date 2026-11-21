# A3 Alternative Concept and Complexity Analysis – VirtualBox (Type-2) instead of KVM (Type-1)

**Question (from the assignment):** *If a type-2 hypervisor such as VirtualBox were used instead of a type-1 hypervisor such as KVM, what would the solution be? Evaluate the overhead on CPU-bound and I/O-bound work, the memory used by the host operating system, and the suitability for production.*

## 1. Note on evidence

**No benchmark results are reported and none are invented.** The comparison is architectural and symbolic. A measurement protocol is given in section 6 so that the numbers can be produced in Part B. Both hypervisors can use VT-x/AMD-V; the difference lies in the layers above the hardware.

## 2. What the alternative solution would be

The portal VMs (`portal-web`, `portal-db`) would run as VirtualBox VMs on a host operating system (for example a Linux or Windows desktop or server edition) instead of as KVM/QEMU VMs on a Linux host dedicated to virtualisation. The guests are unchanged; the layers underneath them change.

| Layer | KVM design | VirtualBox design |
|---|---|---|
| Hardware | Server with VT-x/AMD-V, EPT/NPT | Same |
| Software on hardware | Linux kernel with KVM (hypervisor) | General host OS (Linux, Windows or macOS) |
| VM manager | QEMU (+ libvirt) process per VM | VirtualBox process per VM (VBoxHeadless/VBoxSVC, kernel driver) |
| Guest devices | virtio (disk, network) | Emulated devices or VirtIO/paravirtual options, plus Guest Additions in the guest |
| Management | libvirt, `virsh`, scripts, automation tools | VirtualBox Manager or `VBoxManage` |

See [Diagram D11](diagrams/hypervisor_types.md).

## 3. CPU-bound workload overhead

For code that runs only on the CPU (for example template rendering or computation inside the Python application), both products run the guest at near-native speed with hardware assistance, because the guest instructions execute directly on the CPU. The difference is small and arises mainly from how the vCPU threads are scheduled.

| Factor | KVM | VirtualBox |
|---|---|---|
| Direct execution of guest code | Yes (hardware-assisted) | Yes (hardware-assisted); software mode only if VT-x/AMD-V is unavailable or disabled |
| vCPU scheduling | Linux CFS schedules vCPU threads on a host dedicated to VMs | Host OS scheduler shares the CPU with host applications (desktop, antivirus, browsers) |
| Source of extra overhead | VM exits, nested page walks | Same, plus competition from host processes |

**Assessment:** the CPU-bound overhead should be similar and small for both; the difference grows when the VirtualBox host runs other work or when the host has a busy desktop environment. This is a prediction to be tested.

## 4. I/O-bound workload overhead

I/O is where the architectures differ most. The portal database and the Nginx/Gunicorn network path are I/O-sensitive.

| Path | KVM | VirtualBox |
|---|---|---|
| Disk | virtio-blk/virtio-scsi paravirtual driver; I/O handled by QEMU (or vhost); can use raw or LVM volumes and the host's block layer directly | Virtual disk image file (VDI/VMDK/VHD) on the host file system, so a guest write passes through the guest file system, the virtual controller, the image file, and then the host file system and cache |
| Network | virtio-net with vhost-net (in-kernel data path) or SR-IOV/passthrough with IOMMU | Emulated NIC or paravirtual option, usually NAT or bridged mode through the host network stack; higher per-packet cost |
| Layers a request crosses | Fewer | More |

**Assessment:** every extra layer adds copies and context switches, so I/O-bound overhead is expected to be higher and more variable on VirtualBox, especially for many small disk operations (database commits) and high packet rates (150,000 users logging in). Using paravirtual drivers narrows the gap on both products. Disk image file layouts (dynamically allocated images) also add metadata overhead. The magnitude is **to be measured**.

## 5. Host operating system memory consumption

Memory used by the host is a fixed cost that is not available for guests:

`M_total_used = M_host_OS + M_hypervisor + V · (M_guest + M_app + M_vmm)`

| Item | KVM host | VirtualBox host |
|---|---|---|
| `M_host_OS` | A minimal Linux server install (no GUI), which is what Part 1 designs | A full general-purpose OS, often with a desktop environment and background services |
| `M_hypervisor` | Kernel modules plus per-VM QEMU processes | VirtualBox kernel modules plus per-VM VirtualBox processes |
| Per-VM memory | Guest + QEMU overhead | Guest + VirtualBox process + video/devices (graphical VM defaults) |
| Page-sharing / ballooning | KSM and ballooning available | Balloon is available; page fusion exists but is limited (mostly for Windows guests) |

Both are **O(V)** in the number of VMs and **O(1)** in `M_host_OS`; what changes is the constant `M_host_OS`. A headless Linux KVM host should have the smaller constant. The host memory used is a Part B measurement (`free -m` on both hosts with the same guests idle).

## 6. Measurement protocol for Part B (no results claimed)

1. Same hardware, same guest image, same vCPU count and RAM for both hypervisors; also run the same tests on bare metal as the baseline.
2. CPU-bound: a fixed computation (for example `sysbench cpu`) repeated several times; record mean and spread.
3. I/O-bound: `fio` with random 4 KiB writes with `fsync` (database-like) and sequential reads; network test with `iperf3` between guest and an external machine.
4. Host memory: `free -m` on the host with all guests idle and then loaded.
5. Report overhead as `(T_VM − T_native) / T_native` and state the number of repetitions.

## 7. Suitability for production

| Criterion | KVM | VirtualBox |
|---|---|---|
| Intended use | Servers and cloud platforms | Desktop, development, test, teaching |
| Dedicated host | Yes (minimal server OS) | Usually shares a general-purpose host |
| Automatic start, high availability, live migration | Provided by the surrounding stack (libvirt, cluster managers) | Limited; live migration (teleporting) exists but is not a data-centre feature set |
| Scalability to many VMs and large memory | Designed for it | Not the intended target |
| Management at scale / automation | libvirt API, Ansible and other tools (see Part 1, A2 alternative) | Mainly per-host `VBoxManage` and a GUI |
| Security surface | Minimal host, kernel-level hypervisor, mandatory access control (SELinux/AppArmor via sVirt) | Larger host surface (desktop software), another set of kernel drivers |
| Cost of licence | None for KVM | Base product is open source, extension components have separate terms |
| Verdict for result day | **Suitable** | **Not recommended**: acceptable as a learning and test platform only |

## 8. Decision and complexity summary

| Quantity | KVM | VirtualBox |
|---|---|---|
| Time to run a CPU-bound job | `T_native + small VM-exit cost` | Similar; may vary with host load |
| Time to run an I/O-bound job | Lowest of the two (expected) | Higher and more variable (expected) |
| Memory for V VMs | `M_host_min + V(...)`, O(V) | `M_host_full + V(...)`, O(V), larger constant |
| Management effort for N hosts | O(N), reducible with automation | O(N), mostly manual |
| Licence cost | O(1) (no hypervisor fee) | O(1) for the base product; verify extensions |

**Conclusion:** VirtualBox would run the portal, but the extra host layer increases I/O overhead and memory, and it lacks the operational features needed on result day. KVM is recommended for production; VirtualBox is used only in Part B for convenience.
