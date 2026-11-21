# Task A5.2 – Kubernetes (2 Marks)

**Question (from the assignment):** *Explain pods, deployments, services, replicas, resource requests and limits, and the horizontal pod autoscaler, and how they let the portal scale and recover from failures.*

See [Diagram D15](diagrams/kubernetes_objects.md) and [Diagram D16](diagrams/hpa_and_self_healing.md). No cluster is created in this part; the manifests are written and executed in Part B3.

## 1. What Kubernetes does

Kubernetes is a **declarative orchestrator**: the operator states the **desired state** (for example "three copies of the portal container"), and **controllers** continuously compare it with the **actual state** and make changes until they match (a control loop). The **control plane** (API server, a store of cluster state, scheduler, controller manager) decides; each **worker node** runs a **kubelet** (starts and monitors containers through a container runtime), and a network proxy that implements Services.

## 2. The objects

### 2.1 Pod
The smallest deployable unit: one or more tightly coupled containers that share a network namespace (one IP address) and can share volumes. For the portal, one pod runs the portal container (Nginx/Gunicorn). Pods are **ephemeral**: they are not repaired, they are replaced, and they get a new IP address each time.

### 2.2 Replicas and ReplicaSet
The **replicas** count is the number of identical pods that should be running. A **ReplicaSet** keeps that number: if a pod is deleted or fails, it creates a replacement; if there are too many, it removes some. Replicas provide both **capacity** (load is shared) and **redundancy** (one failure leaves the others serving).

### 2.3 Deployment
A **Deployment** manages ReplicaSets for stateless applications. It declares the pod template (image, port, probes, resources), the number of replicas and the label selector that identifies its pods. Updating the image creates a new ReplicaSet and performs a **rolling update**, replacing pods gradually (controlled by `maxSurge` and `maxUnavailable`) and allowing **rollback**; so a new portal version can be released without downtime.

### 2.4 Service
Pods come and go, so clients need a stable address. A **Service** gives a stable virtual IP and DNS name and forwards traffic to the pods that match its **label selector**, balancing across them. Only pods that are **ready** (passing the readiness probe) are listed as endpoints. Types: `ClusterIP` (internal), `NodePort`, and `LoadBalancer`, which asks the cloud provider to create the external load balancer of the A5.1 design. (An ingress controller can provide layer-7 routing.)

### 2.5 Resource requests and limits
Every container may declare:

| Setting | Meaning | Enforced by |
|---|---|---|
| **Request** | The amount of CPU/memory the container is guaranteed and that the **scheduler** reserves when choosing a node | Scheduler (placement); CPU request also sets the relative CPU share (`cpu.weight`) |
| **Limit** | The maximum the container may use | Kernel through **cgroups** (see A4.1): CPU above the limit is **throttled** (`cpu.max`), memory above the limit causes the container to be **OOM-killed** and restarted (`memory.max`) |

Consequences: requests that are too low allow too many pods on a node and cause contention; requests that are too high waste capacity and leave pods unscheduled; a memory limit that is too low makes the portal restart under load; a CPU limit that is too low throttles it and raises latency. Pods are assigned a **Quality-of-Service class** from these values (Guaranteed when requests equal limits, Burstable, BestEffort), which decides who is evicted first when a node is short of memory. Requests also matter for autoscaling, because CPU utilisation is measured **relative to the request**.

### 2.6 Probes (supporting features)
A **readiness probe** controls whether a pod receives traffic; a **liveness probe** tells the kubelet to restart a container that is stuck. They make failure detection automatic.

### 2.7 Horizontal Pod Autoscaler (HPA)
The HPA adjusts the replica count of a Deployment according to observed metrics (usually average CPU utilisation, relative to requests, from the metrics service). Control loop (default period about 15 seconds):

`desiredReplicas = ceil( currentReplicas × currentMetric / targetMetric )`

bounded by the configured **minimum** and **maximum** replicas, with a small tolerance to avoid flapping and a **stabilisation window** before scaling down (by default several minutes), so that a short dip does not remove capacity. Example (symbolic): with a target of 60% CPU and a current average of 90% on 10 replicas, `desired = ceil(10 × 90/60) = 15`. The HPA scales the **number of pods**; if the new pods do not fit on existing nodes they remain *Pending* until a **node (cluster) autoscaler** adds worker VMs, which takes longer than starting a pod. The HPA cannot scale a database.

## 3. How these features give scaling

1. **Horizontal scaling:** the Deployment's replicas are the unit of capacity; the Service spreads the load over them.
2. **Automatic scaling:** the HPA raises replicas when utilisation exceeds its target and lowers them afterwards; the node autoscaler adds VMs when the pods do not fit.
3. **Predictable placement:** requests let the scheduler place pods on nodes that have the capacity; limits stop one pod from starving others.
4. **Fast start:** pods are containers from a local image (A4.2), so new replicas become ready in seconds once a node is available.
5. **Pre-warming:** setting a higher minimum before result day compensates for the reaction delay of autoscaling (A5.1, section 4.4).

## 4. How these features give recovery from failures

| Failure | Detection | Automatic action |
|---|---|---|
| Portal process crashes or becomes stuck | Container exits, or liveness probe fails | kubelet restarts the container (restart policy) |
| Pod not yet able to serve, or becomes unhealthy | Readiness probe fails | Pod is removed from the Service endpoints, so no requests are sent to it; it returns when ready |
| Pod deleted or evicted | ReplicaSet notices fewer pods than desired | ReplicaSet creates a new pod, scheduled on any node with capacity |
| Memory limit exceeded | Kernel OOM kill | Container is restarted; the memory limit and the request are then reviewed |
| Worker node fails | Node reports stop; control plane marks it not ready after a grace period | After a toleration period (default several minutes) pods are evicted and recreated on other nodes; replicas on other nodes serve meanwhile |
| Zone failure | Nodes in that zone become not ready | Remaining replicas in other zones serve; autoscaling and rescheduling restore capacity (needs spare quota and capacity) |
| Bad release | Readiness probes fail during the rolling update | Rollout stops with old pods still serving; rollback to the previous ReplicaSet |

The recovery speed is bounded by the detection delays (probe periods, node-grace and toleration periods) and by the time to start replacement pods; during these delays the remaining replicas must carry the load, which is why **spare headroom** (`ρ_target` below 1) and **replicas spread across nodes and zones** are part of the design. Stateful parts (the database) are **not** recovered this way: they rely on the managed database's standby, backups and persistent storage.

## 5. Mapping to the portal

| Portal need | Kubernetes feature |
|---|---|
| Same application image on every instance | Pod template in the Deployment |
| Share 150,000 logins over many instances | Replicas + Service (+ cloud load balancer) |
| React to rising load | HPA (+ node autoscaler) |
| Stop one instance starving others | Resource limits |
| Fit the right number of pods on each node | Resource requests |
| Survive a crash, a node or a bad release | ReplicaSet, probes, rolling update, rollback |
| Persistent data | Managed database outside the cluster |

## 6. Evaluation

Kubernetes provides, from a few declarations, the scaling and recovery that would otherwise require custom scripts around VMs. Its limits are important: it adds operating complexity and learning effort, autoscaling is **reactive** and may be slower than a sudden peak, requests and limits need measured values, and the database remains the single point that must be designed separately. Comparing this with fixed VMs is the next file, [A5_Fixed_VM_vs_Autoscaling.md](A5_Fixed_VM_vs_Autoscaling.md).
