# Pod deletion and recovery (ACTUAL, from `actual_output.txt`)

`kubectl apply -f deployment.yaml` → 3/3 pods Running, labels `app=result-portal`, QoS Burstable; `kubectl describe pod` shows
Limits cpu 500m / memory 512Mi and Requests cpu 250m / memory 256Mi.

`kubectl delete pod result-portal-86c9577d7f-cgww4` at 07:03:44:
* immediately: old pod `Terminating`, a **new pod `result-portal-86c9577d7f-zbst6` created (Pending)** by the ReplicaSet,
* t+2 s: `ContainerCreating`, t+3 s: `1/1 Running`.
Deployment returned to 3/3 without any manual action. Recovery time for a deleted pod on a node with the image cached ≈ 3 s.
(A failed *node* would add the node-failure detection time, 40 s by default, before pods are rescheduled – not tested, single-node cluster.)
