# B4.3 – Production decision

**Decision: the university's own servers running containers on Kubernetes, with the public cloud kept only as a contingency (burst/disaster recovery) that is switched on if measured capacity is insufficient.**

## Why (evidence)
1. **Demand is modest per container.** One measured container handles 2,005.6 rps on a single vCPU (31 MiB). For 150,000 students, k = 10 requests each and a burst factor of 10, 3 containers suffice without a database (D=1) and about 21 if each request costs 10x more CPU (D=10, an assumption). Even the pessimistic case is tens of cores and a few GiB – plausible for an existing server room, which matches the requirement of not buying new servers.
2. **Shared infrastructure.** The LMS, exams and portal must share the hardware. The measured extension showed Kubernetes enforcing per-service requests and limits (portal 250m/500m, notice-service 100m/200m) and refusing to schedule when requests exceed the node (`Insufficient cpu`), which is the admission control needed to protect exams on result day.
3. **Recovery.** A deleted pod was replaced in about 3 s; VMs would add a guest OS per instance (VM numbers are PENDING, so the argument rests on the architecture, not a measurement) and Compose cannot survive a server failure.
4. **Student-data control.** Results are personal data; keeping them on university hardware avoids third-party hosting.
5. **Cost.** Own servers are already paid for, so the marginal cost is E + S (see `cloud_cost_comparison.md`). Cloud-only needs a real price p; the formula shows a fixed result-day fleet costs about N/n0 times an autoscaled one, so cloud-only is viable only with autoscaling.

## Options compared
| Option | Verdict |
|---|---|
| Own servers + VMs | Works, but more memory per instance and slower scale-up; use for strong isolation only. |
| **Own servers + containers/Kubernetes** | **Chosen.** |
| Public cloud only | Fastest to add capacity, but recurring cost and data-control concerns; no price was invented here. |
| Hybrid | Adopt only as contingency: add cloud nodes if load tests with the real database show N above server-room capacity. |

## Conditions that would change the decision
D measured with PostgreSQL is much higher than 10; the server room has fewer free cores than N; a real quote gives p* above the break-even in `cloud_cost_comparison.md`; the VM measurements (pending) show an unexpected advantage.
