# Task A3.1 – Hypervisors (3 Marks)

**Question (from the assignment):** *Explain type-1 (bare-metal: KVM, VMware ESXi, Xen, Hyper-V) and type-2 (hosted: VirtualBox, VMware Workstation) hypervisors, full virtualisation and para-virtualisation, and hardware support (Intel VT-x, AMD-V).*

Case study: the University Result Portal (a Python web application and a database; about 150,000 students log in within two hours on result day). See [Diagram D11](diagrams/hypervisor_types.md).

## 1. What a hypervisor does

A **hypervisor** (virtual machine monitor, VMM) is the software layer that lets several **virtual machines (VMs)** share one physical computer. It gives each VM virtual CPUs, a share of memory, and virtual devices (disk, network), and it keeps the VMs separated. Each VM runs its own **guest operating system** with its own kernel. Popek and Goldberg (1974) set the classical requirements: a VMM must provide an environment *equivalent* to the real machine, keep *control* of the hardware resources, and run most guest instructions directly on the CPU for *efficiency*.

Two terms are used throughout: the **host** is the physical machine (and the software that owns the hardware); a **guest** is a VM and the operating system inside it.

## 2. Type-1 and type-2 hypervisors

| Property | Type-1 (bare-metal) | Type-2 (hosted) |
|---|---|---|
| Position | Runs directly on the hardware | Runs as an application on a general-purpose host OS |
| Who schedules the hardware | The hypervisor (or its privileged component) | The host OS schedules the hypervisor like any other process |
| Layers between guest and hardware | Guest, hypervisor, hardware | Guest, hypervisor application, host OS, hardware |
| Typical use | Data centres, clouds, production servers | Desktops, laptops, teaching, development and testing |
| Examples (from the assignment) | KVM, VMware ESXi, Xen, Hyper-V | VirtualBox, VMware Workstation |

A type-1 hypervisor has fewer layers, so less overhead, a smaller trusted code base for the hypervisor itself, and no desktop software to patch. A type-2 hypervisor is easy to install and use because the host OS already provides drivers, file systems and a user interface, but every guest request also passes through the host OS.

## 3. The named hypervisors

**KVM (Kernel-based Virtual Machine).** KVM is a module of the Linux kernel (`kvm.ko` with `kvm-intel` or `kvm-amd`) that uses the processor's virtualisation extensions to turn the Linux kernel itself into a hypervisor. Each VM is an ordinary Linux process (usually **QEMU**); each virtual CPU is a thread of that process, so the Linux scheduler (CFS, see Part 1, A1.2), memory manager and drivers are reused. QEMU supplies device emulation, and **virtio** drivers give near-native I/O. KVM is classified as type-1 because the hypervisor code runs in the kernel with direct hardware control, even though the kernel is a full general-purpose OS (some authors call it "hosted-like"; the practical result is bare-metal behaviour). This is the most natural fit for the university's Linux skills (Part 1).

**VMware ESXi.** A commercial bare-metal hypervisor from VMware (now part of Broadcom). Its own small kernel, the VMkernel, runs on the hardware and hosts the VMs. It is managed through vCenter and provides mature features such as live migration (vMotion) and high availability. Its licensing model has changed in recent years, so the current terms must be checked before costing (see A3.2 and [references.md](references.md)).

**Xen.** An open-source type-1 hypervisor that runs directly on the hardware as a thin layer. A privileged VM called **Dom0** controls the hardware and management; unprivileged guests are **DomU**. Xen supports para-virtualised guests (modified kernels using hypercalls), hardware-assisted full virtualisation (HVM), and a hybrid mode (PVH) (Barham et al., 2003).

**Microsoft Hyper-V.** A type-1 hypervisor. When the Hyper-V role is enabled, the hypervisor starts first and the Windows installation becomes the privileged **root partition**; other VMs are **child partitions**. Guests use synthetic devices over the VMBus (Microsoft calls this "enlightenment"), which is a para-virtualised path. It is licensed as part of Windows Server and available on some Windows client editions.

**Oracle VirtualBox.** A type-2 hypervisor that runs on Windows, Linux and macOS hosts. The base product is open source (GPL); some add-ons (the Extension Pack) are under a separate licence, so the terms must be checked for institutional use. It can use VT-x or AMD-V when available. It is convenient for coursework such as Part B of this assignment.

**VMware Workstation.** A type-2 desktop hypervisor for Windows and Linux hosts, used for development and testing. Its licensing for personal and commercial use has changed recently and must be verified.

## 4. Full virtualisation and para-virtualisation

| Aspect | Full virtualisation | Para-virtualisation |
|---|---|---|
| Guest OS | Unmodified; unaware that it is virtualised | Modified, or equipped with special drivers, so that it knows it is virtualised |
| Privileged operations | Trapped and emulated by the hypervisor (binary translation in early VMware products; hardware assistance today) | Guest calls the hypervisor directly with **hypercalls** |
| Devices | Emulated real devices (for example an emulated disk or network card): simple but slow | Paravirtual devices such as **virtio** (KVM), Xen split drivers, Hyper-V VMBus: fast |
| Advantage | Runs any OS, including proprietary ones | Less overhead, especially for I/O |
| Disadvantage | More overhead without hardware help | Needs guest support |

In practice the two are **combined**: modern KVM, Xen (PVH/HVM with PV drivers) and Hyper-V run an unmodified guest kernel with hardware assistance for CPU and memory, and install paravirtual drivers (virtio and equivalents) for disk and network. For the portal, a Linux guest with virtio disk and network drivers is the recommended combination.

## 5. Hardware support: Intel VT-x and AMD-V

Early x86 processors had instructions that could not be trapped safely, which made full virtualisation difficult. Since about 2005–2006 both vendors added extensions that remove the problem.

| Feature | Intel | AMD |
|---|---|---|
| CPU virtualisation | **VT-x** (VMX): adds *VMX root* mode for the hypervisor and *VMX non-root* mode for the guest; a **VMCS** structure holds guest and host state; **VM entry** and **VM exit** switch between them | **AMD-V** (SVM): `VMRUN` enters the guest, `#VMEXIT` returns; a **VMCB** holds the state |
| Nested/extended page tables (memory virtualisation) | **EPT** | **NPT** (also called RVI) |
| I/O virtualisation (IOMMU) | **VT-d** | **AMD-Vi** |
| Linux CPU flag | `vmx` | `svm` |

How they help: the guest kernel runs in its normal privilege rings inside non-root (guest) mode, so most instructions run directly on the CPU at full speed. Only sensitive events (for example certain privileged instructions, I/O accesses and page faults the hypervisor must handle) cause a **VM exit**, which is the main source of CPU virtualisation overhead. With EPT/NPT the hardware walks both the guest page tables and the hypervisor's tables, which removes the need for the hypervisor to maintain "shadow" page tables in software. The IOMMU lets a device be assigned directly to a VM safely.

A Linux host can check support with `grep -E 'vmx|svm' /proc/cpuinfo`, and KVM needs the feature enabled in the firmware (BIOS/UEFI) settings. (These commands are named for use in Part B; no output is claimed here.)

## 6. Application to the portal

The university's own servers would use a **type-1 hypervisor** (KVM is the natural choice with Linux staff; ESXi or Hyper-V would require licences and different skills). Type-2 products are suitable for the student's own laptop in Part B but not for serving 150,000 users. The portal guest would be a Linux VM using virtio devices, on a host with VT-x/AMD-V and EPT/NPT enabled. The benefits and costs of this choice are evaluated in [A3_Virtualisation_Benefits_Costs.md](A3_Virtualisation_Benefits_Costs.md), and the VirtualBox-versus-KVM comparison is in [A3_Type1_vs_Type2_Analysis.md](A3_Type1_vs_Type2_Analysis.md).
