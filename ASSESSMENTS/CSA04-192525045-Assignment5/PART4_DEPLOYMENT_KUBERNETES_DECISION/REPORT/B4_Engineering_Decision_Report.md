# B4 – Final Engineering Decision (5 marks)
S. Sachin, 192525045, CSA04.

## B4.1 Practical extension (ACTUAL)
`notice-service` (separate image, Deployment and Service) runs beside the portal: portal 250m/500m CPU and 256Mi/512Mi memory; notice-service 100m/200m and 128Mi/256Mi. Both Running. With the portal at 3 replicas the second service was Pending (`Insufficient cpu`); the portal was scaled to 2 on this 1-CPU node. Evidence: `Task03/actual_output.txt`.

## B4.2 Capacity
Measured 2,005.6 rps per container. 7,200 s window. N = ceil(R_peak / (rps x u)). With ASSUMPTIONS k = 10 requests per student, peak factor 10, utilisation 0.5: 3 containers if no database cost (4 with a spare), about 21 if requests cost 10x more with a database. Full table and how to recompute: `Task04/capacity_estimation.md`.

## B4.3 Decision
Own servers with containers and Kubernetes, with the public cloud only as a contingency if load tests with the real database exceed server-room capacity. Reasons: measured per-container capacity is small relative to demand, Kubernetes enforces per-service resources for the shared LMS/exam workload, pod recovery was about 3 s, student data stays on university hardware, and marginal cost of owned servers is operating cost only. Cloud cost comparison is symbolic (no prices invented): `Task04/cloud_cost_comparison.md`. Limits: VM measurements pending, database factor D unmeasured.
