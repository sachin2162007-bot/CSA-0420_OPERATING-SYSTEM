# Consolidated Complexity Analysis – Part 2 (A3, A4 and A5)

Symbols: V = number of VMs, N = number of portal instances (containers or VMs), C = containers per host, L = number of layers in an image, S = image size, B = network bandwidth, M = memory size of a VM, D = rate at which a VM dirties memory, s = number of Services, p = pods, n = nodes, R = replicas, λ = arrival rate, μ = service rate per instance, ρ = utilisation, H = hours per year. **No measurements or prices are reported;** cells marked "measure" are to be filled in from Part B.

## 1. A3 – Virtualisation

| Mechanism | Time | Space | Note |
|---|---|---|---|
| Guest instruction (hardware assisted) | O(1) per instruction, native speed | – | Only sensitive events cause a VM exit |
| VM exit handling | O(1) per exit, cost measured | – | `T_vm = T_native + exits × cost_per_exit + I/O emulation` |
| Memory for V VMs | – | O(V), `M_host + V(M_guest + M_app + M_vmm)` | Guest OS duplicated per VM |
| Live migration (pre-copy) | About O(M / (B − D)) if the dirty rate D is below the bandwidth B; does not converge if D ≥ B | O(M) temporary | Clark et al. (2005) |
| Snapshot creation | O(1) (copy-on-write metadata) | Grows with changed blocks | Long chains slow I/O |
| Hypervisor licences | O(1) for KVM; O(hosts or cores) for commercial products | – | No price stated |
| Type-2 vs type-1 | Same CPU-bound order; extra layer on I/O path | Larger constant `M_host_OS` | Measure with the protocol in A3 analysis |

## 2. A4 – Containerisation

| Quantity | Time | Space | Note |
|---|---|---|---|
| Start a container (image local) | O(1), around seconds or less (measure) | O(1) writable layer | No guest OS boot |
| Pull an image to a new node | O(S / B), only missing layers | O(S) on disk, shared by all containers of the image | Layer digests allow reuse |
| Start k containers on a node with the image | O(k) | O(k · M_app) + shared kernel | |
| Start k VMs | O(k · T_boot) (minutes each) | O(k · (M_guestOS + M_app)) | A4 alternative |
| File lookup through overlayfs | O(L) in the worst case (layers searched top to bottom) | – | Keep L small |
| Patching N instances | O(1) rebuild and redeploy (containers); O(N) guest OS updates (VMs) | – | |
| Cgroup limit check | O(1) per accounting event | O(1) per cgroup | CPU throttling, memory limit |

## 3. A5 – Cloud and Kubernetes

| Mechanism | Time | Space | Note |
|---|---|---|---|
| Required replicas | `R = ceil(λ·k / (μ·ρ_target))`, O(1) to compute | O(R) pods | μ measured in Part B |
| HPA decision | O(1) per cycle (one formula per target) | O(1) | Default period about 15 s; reaction delay dominates |
| Scheduling a pod | About O(n) per pod (filter and score nodes) | O(p + n) cluster state | Fast for small clusters |
| Service routing (iptables mode of kube-proxy) | O(s) rule traversal per new connection | O(s) rules | IPVS mode uses hashing, O(1) |
| Failure recovery of a pod | Detection delay + start (seconds) | – | Needs spare node capacity |
| Failure recovery of a node | Grace and toleration periods (minutes) + pod start | – | Replicas on other nodes serve meanwhile |
| Fixed VM fleet compute | – | `O(V_peak · H)` VM-hours | Idle most of the year |
| Autoscaled compute | – | `O(V_min · H + extra · h_peak)` | Plus management overhead |
| Response time as `ρ → 1` | Grows roughly like `1/(1 − ρ)` | – | Reason for headroom |

## 4. Summary of the three alternatives

| Alternative (assignment) | Main effect on the portal | Which grows faster |
|---|---|---|
| VirtualBox instead of KVM | Higher I/O overhead and host memory, unsuitable for production | Constant factors, not the order |
| VMs instead of containers | Slower start (minutes), memory repeated per guest OS, stronger isolation | Memory and start time per instance |
| Fixed VMs instead of Kubernetes autoscaling | Predictable on the day, but large idle capacity and higher cost | Cost: `V_peak · H` versus `V_min · H` plus peak extras |

## 5. Remaining step

Replace every "measure" with values from Part B (B3.2 start-up time, memory and requests per second; B4.2 capacity) and take any price from the provider's current price list; cite them in [references.md](references.md).
