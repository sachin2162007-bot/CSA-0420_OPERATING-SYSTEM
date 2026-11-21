# Task A4.1 – How Containers Work (2 Marks)

**Question (from the assignment):** *Explain how containers use Linux namespaces and control groups (cgroups), and how a container image is built in layers.*

Case study: the University Result Portal. See [Diagram D12](diagrams/vm_vs_container_stack.md) and [Diagram D13](diagrams/container_image_layers.md).

## 1. The central idea

A container is **not a small virtual machine**. It is an ordinary group of Linux processes that the kernel has been told to (a) **show a restricted view** of the system, using **namespaces**, and (b) **limit in resource use**, using **control groups (cgroups)**. The container's file system is built from a read-only **image** plus a thin writable layer. All containers on a host share **one kernel**; there is no guest operating system and no hardware emulation.

`container = process(es) + namespaces (what it can see) + cgroups (what it can use) + image file system (what it contains)`

## 2. Linux namespaces – isolation of what a process can see

A namespace wraps a global kernel resource so that processes inside it see their own private instance. A process is placed in new namespaces when it is created with `clone()` and namespace flags, can move itself with `unshare()`, and another process can join existing ones with `setns()`. The main types are:

| Namespace | Isolates | Effect in the portal container |
|---|---|---|
| **PID** | Process ID numbers | Gunicorn sees itself as PID 1 and cannot see host processes |
| **Mount (mnt)** | Mount points and the root file system | The container sees only its image file system, not the host's `/etc` |
| **Network (net)** | Interfaces, IP addresses, routing tables, ports | Its own virtual Ethernet interface and its own port 8000; the host publishes it with port mapping |
| **UTS** | Host name and domain name | The container has its own host name |
| **IPC** | System V IPC and POSIX message queues | Shared-memory segments are private |
| **User** | User and group IDs | UID 0 inside can be mapped to an unprivileged UID outside (rootless containers) |
| **Cgroup** | The view of the cgroup tree | The container sees its own cgroup as the root |
| **Time** | Boot and monotonic clock offsets (recent kernels) | Rarely needed |

Namespaces provide **isolation of visibility**, not a limit on consumption: a process in its own PID namespace could still use all the CPU unless cgroups prevent it.

## 3. Control groups (cgroups) – limits and accounting

Cgroups organise processes into a hierarchy and let the kernel **limit, prioritise, account for and control** the resources the group uses. Modern Linux uses **cgroup v2**, with a single unified hierarchy under `/sys/fs/cgroup` (which can be observed with the `/sys` tools described in Part 1, A1.2). Each controller exposes files:

| Resource | cgroup v2 control (examples) | Behaviour when the limit is reached |
|---|---|---|
| CPU | `cpu.max` (quota and period), `cpu.weight` (relative share) | The group is **throttled**: it must wait until the next period |
| Memory | `memory.max` (hard limit), `memory.high` (throttling threshold) | The kernel reclaims memory; if it cannot, the **OOM killer** terminates a process in the group |
| Processes | `pids.max` | `fork()` fails (prevents fork bombs) |
| Block I/O | `io.max`, `io.weight` | I/O is throttled or weighted |

Container tools translate their options into these files. For example, a CPU limit such as `--cpus` becomes a quota per period in `cpu.max`, and `--memory` becomes `memory.max`. Kubernetes resource limits (A5.2) use the same mechanism. Accounting files such as `memory.current` and `cpu.stat` feed the monitoring commands (`docker stats`) used in Part B.

**Relationship:** namespaces answer *"what can this process see?"*; cgroups answer *"how much can this process use?"*. Security features such as Linux **capabilities**, **seccomp** system-call filters and mandatory access control (SELinux/AppArmor) are added on top to restrict what the container may do (see A4.2).

## 4. How container images are built in layers

An **image** is a packaged, read-only file system plus metadata (default command, environment, exposed port, user). It follows the Open Container Initiative (OCI) image format: a **manifest** lists an ordered set of **layers** and a **configuration**; each layer is a compressed archive of file-system changes (files added, changed or deleted compared with the layer below) and is identified by a **content hash** (SHA-256 digest).

### 4.1 Building

A build file (Dockerfile) is read top to bottom. Instructions that change files (for example installing packages or copying files) **create a new layer**; instructions that only set metadata (default command, port, user) add configuration but no file-system content. Typical layer stack for the portal image (conceptual):

| Order | Layer content | Changes how often |
|---|---|---|
| 1 (bottom) | Base image: minimal Linux user-space and the Python runtime | Rarely |
| 2 | System packages needed by the application | Rarely |
| 3 | Python dependencies (installed from a requirements list) | When dependencies change |
| 4 | Application source code | Every release |
| 5 (top, at run time) | Thin **writable container layer** | Every run; discarded with the container |

**Build cache:** a layer is rebuilt only if its instruction or inputs changed, and all layers above it are then rebuilt. Therefore the instructions are ordered **from least to most frequently changed**: dependencies before application code, so that a code change reuses the cached dependency layer. This shortens builds and, because unchanged layers are shared, also shortens image **pulls and pushes**: a node that already has layers 1–3 downloads only the changed layer 4.

### 4.2 Running: union file system and copy-on-write

At run time the container engine presents the stacked layers as one file system, typically with **overlayfs**: read-only **lowerdir** layers, a writable **upperdir**, and a merged view. Reading a file finds it in the highest layer that contains it. Writing to a file that exists in a lower layer first **copies it up** to the writable layer (**copy-on-write**); deleting a lower-layer file is recorded with a *whiteout* marker. Consequences:

- Many containers started from the same image **share** the lower layers on disk and in the page cache, so extra instances cost little disk space.
- Data written in the writable layer **disappears** when the container is removed, so the database's data must use a **volume** or a persistent volume (A4.2, A5).
- Writes to the overlay have an extra cost compared with writing to a volume, which matters for databases.
- A deep layer stack slightly lengthens lookups, so builds avoid many unnecessary layers and avoid leaving temporary files in earlier layers (deleting a file in a later layer does not shrink the image).

### 4.3 Why this matters for result day

Images are **immutable and identical** on every node, so the 20th portal replica behaves like the first, and a new node can start serving after pulling the image once (`O(S/B)` for image size S and network bandwidth B, then `O(1)` per additional container on that node). The orchestration of these containers is covered in [A5_Kubernetes_Architecture.md](A5_Kubernetes_Architecture.md).

## 5. Summary

| Concept | Kernel feature | Purpose |
|---|---|---|
| What a container sees | Namespaces (PID, mount, network, UTS, IPC, user, cgroup, time) | Isolation |
| What a container may use | cgroups (cpu, memory, pids, io) | Limits and accounting |
| What a container contains | Layered, content-addressed OCI image with overlayfs | Reproducible, shareable, quickly deployable file system |
