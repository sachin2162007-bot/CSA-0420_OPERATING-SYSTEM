# B4.1 – Evidence (ACTUAL, `actual_output.txt`)

| Service | Replicas | CPU request / limit | Memory request / limit |
|---|---|---|---|
| result-portal (official YAML, B3.3) | 2 (scaled from 3 for this step) | 250m / 500m | 256Mi / 512Mi |
| notice-service | 1 | 100m / 200m | 128Mi / 256Mi |

Both Running; `curl` to the portal `/health` returned `{"status":"ok"}`, to notice-service `/notices` returned its JSON; `whoami` in the notice pod printed `notice`.

**Capacity finding:** with the portal at 3 replicas (750m) the node had 950m of 1000m CPU requested, so the new service stayed Pending
(`Insufficient cpu`, kept in `actual_output_first_attempt_pending.txt`). Reducing the portal to 2 replicas on this 1-CPU node let both run (800m requested).
This is exactly the sharing problem in the case study: LMS/exam services and the portal compete for the same requested capacity, and requests must be planned per node.
The official `deployment.yaml` was not edited; the change was `kubectl scale deployment/result-portal --replicas=2`.
