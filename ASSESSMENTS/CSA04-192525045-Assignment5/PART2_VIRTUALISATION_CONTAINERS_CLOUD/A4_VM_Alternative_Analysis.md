# A4 Alternative Concept and Complexity Analysis – Portal and Database in Separate VMs

**Question (from the assignment):** *If the portal and the database were each run in their own virtual machine instead of in containers, what would the solution be? Evaluate the start-up time (minutes versus seconds), the memory used per instance (a full guest OS versus shared kernel) and the strength of isolation.*

## 1. Note on evidence

**No measurements or benchmark results are reported.** The analysis is architectural and symbolic; the real values are measured in Part B3.2 (start-up time with a stopwatch or log timestamps, memory with `free -m` and `docker stats`).

## 2. What the VM-based solution would be

| Element | Container design | VM design |
|---|---|---|
| Portal | N containers from one image, on shared nodes | N identical `portal-web` VMs cloned from a template, each running its own guest Linux, Nginx, Gunicorn and Python |
| Database | One PostgreSQL container with a volume (or a managed service) | One `portal-db` VM (the design of Part 1, A2.2: 10.20.20.10 in the internal subnet) |
| Packaging | Image from a build file | VM template or disk image prepared by automation (Part 1, A2 alternative: Ansible) |
| Scaling out | Start more containers | Clone and boot more VMs; add each to the load balancer after health checks |
| Network | Virtual bridge between containers | Virtual switch and subnets between VMs, firewall rules per VM |

Scale-out of the VM design, in structured pseudocode (the SIMATS pseudocode standard is not included in the supplied PDF; this uses a plain structured style that can be adapted to it):

```
PROCEDURE ScaleOutVM(k)
    FOR i FROM 1 TO k DO
        vm <- CloneFromTemplate(portal_template)
        PowerOn(vm)
        WAIT UNTIL GuestOperatingSystemBooted(vm)
        WAIT UNTIL ServiceHealthy(vm, "/health")
        AddToLoadBalancer(vm)
    END FOR
END PROCEDURE

PROCEDURE ScaleOutContainer(k)
    EnsureImagePresentOnNode(image)        // pulled once per node
    FOR i FROM 1 TO k DO
        c <- StartContainer(image)
        WAIT UNTIL ServiceHealthy(c, "/health")
        AddToLoadBalancer(c)
    END FOR
END PROCEDURE
```

## 3. Start-up time

`T_start_VM = T_clone + T_firmware + T_kernel_boot + T_init_services + T_app_start`
`T_start_container = T_image_pull (first time on a node) + T_create_namespaces_cgroups + T_app_start`

| Quantity | VM | Container |
|---|---|---|
| Per-instance start (image already local) | Firmware, kernel and operating-system boot dominate: minutes (as stated in the assignment; the order is tens of seconds to minutes) | Application start dominates: seconds or less |
| First start on a new host | Copy or clone a disk image of many GB | Pull only the layers the node lacks |
| Time to add k instances | `O(k · T_start_VM)` sequentially, divided by the parallelism limit | `O(k · T_start_container)`, much smaller constant |

**Result-day effect:** if demand rises within minutes, the VM design cannot react in time, so VMs must be **started in advance** (and paid for while idle), whereas containers can follow the demand (see [A5_Fixed_VM_vs_Autoscaling.md](A5_Fixed_VM_vs_Autoscaling.md)).

## 4. Memory used per instance: full guest OS versus shared kernel

Let N be the number of portal instances.

- VM design: `M_VM = M_host + N·(M_guestOS + M_portal + M_vmm) + (M_guestOS + M_db + M_vmm)`
- Container design: `M_ctr = M_host + M_kernel_shared + N·(M_portal + M_runtime) + M_db`

The difference is approximately `N · (M_guestOS + M_vmm − M_runtime)`: the **guest OS memory is repeated N times**. A guest also keeps its own page cache for files that the others may also cache (duplicated cache), and its own services (journald, cron, SSH, and so on). Memory growth is **O(N)** for both, but the constant per instance is much larger for VMs. A full guest OS also means **more disk per instance** and **more packages to patch** (each VM has its own kernel and packages to keep updated, O(N) patching), while a container image is rebuilt once and redeployed.

Techniques such as ballooning and KSM reduce, but do not remove, this duplication. For the database, the VM's guest overhead is small compared with the large memory that the database itself needs, so for **one** instance the difference is unimportant; the difference matters for **many** portal instances.

## 5. Isolation strength

| Aspect | VM design | Container design |
|---|---|---|
| Kernel | Separate guest kernels for portal and database | One shared host kernel |
| Compromise of the portal | Attacker must also escape the hypervisor to reach the database VM; kernel exploits remain inside the guest | A kernel vulnerability or misconfiguration (for example a privileged container) could reach the host and the other containers |
| Resource isolation | Fixed virtual hardware per VM, hypervisor scheduling | cgroups limits; noisy-neighbour effects are possible on shared kernel structures |
| Blast radius of a crash | Kernel panic affects one VM | Kernel panic on the node affects all containers on that node |
| Network separation | Distinct virtual NICs and subnets | Virtual networks and network policies |

The VM design has the **stronger** isolation. For the portal, the database (the valuable asset) benefits from this boundary, which is why [A4_Containers_vs_Virtual_Machines.md](A4_Containers_vs_Virtual_Machines.md) recommends a VM or managed service for the database. For the stateless web tier the extra isolation does not justify the memory and start-up cost, provided containers are hardened.

## 6. Complexity summary

| Quantity | VM design | Container design |
|---|---|---|
| Time to start k new instances | `O(k·T_VM_boot)` | `O(k·T_ctr_start)`, plus one image pull per node |
| Memory for N instances | `O(N·(M_guestOS + M_app))` | `O(N·M_app) + O(1)` shared kernel |
| Disk for N instances | `O(N·S_disk)` (copy-on-write clones reduce) | `O(S_image + N·S_writable)` (layers shared) |
| Patching effort | `O(N)` guest operating systems | `O(1)` rebuild of one image, then redeploy |
| Isolation | Stronger | Weaker, improved by hardening |

## 7. Conclusion

Running the portal and database in separate VMs would work and gives strong isolation, but it multiplies guest-OS memory, makes scale-out take minutes, and requires patching every guest. It is a sound choice for the **database**, and for the cluster nodes that host containers, but not for the **replicated portal tier** that must expand quickly on result day.
