# A5 Alternative Concept and Complexity Analysis – Fixed Virtual Machines Sized for Result Day

**Question (from the assignment):** *If the cloud deployment used fixed virtual machines sized for result day instead of Kubernetes with an autoscaler, what would the solution be? Evaluate the response time on result day, the resources left idle on the other 364 days, and the cost.*

## 1. Note on evidence

**No prices and no benchmark results are used.** Cost is expressed as VM-hours, and the numbers in section 6 are **hypothetical parameters that illustrate the method**, not measurements or quotations. They must be replaced by the capacity measured in Part B4.2 and by the provider's current price list (see [references.md](references.md)).

## 2. What the fixed-VM solution would be

| Element | Kubernetes with autoscaling (A5.1) | Fixed VMs |
|---|---|---|
| Portal tier | Pods that scale between a minimum and a maximum | `V_peak` identical VMs (portal-web, as in Part 1) running all year, each with Nginx and Gunicorn |
| Capacity decision | Continuous, by the autoscaler | One decision before result day, based on the estimated peak |
| Load balancer | Managed load balancer | Same managed load balancer, listing the VMs |
| Database | Managed database | Same managed database (the database is sized for peak in both designs) |
| Recovery | Pods rescheduled automatically | Load balancer removes unhealthy VMs; a new VM must be created and booted (manually or by a scaling group) |
| Releases | Rolling update | Replace VMs one by one or in batches |

Sizing rule: `V_peak = ceil( λ_peak · k / (μ_vm · ρ_target) ) + V_spare`, where `μ_vm` is the sustainable requests per second of one VM, and `V_spare` covers failure of a VM or a zone.

## 3. Response time on result day

Model a service with `c` servers, arrival rate `λ`, and service rate `μ` per server. The utilisation is `ρ = λ/(c·μ)`. For ρ well below 1 the response time is close to the service time; as ρ approaches 1, queueing delay increases sharply (for the simple single-queue model it grows roughly like `1/(1 − ρ)`), and above 1 the queue grows and requests time out.

| Aspect | Fixed VMs | Kubernetes autoscaling |
|---|---|---|
| Capacity at the first minute of the peak | **Full**, as the capacity already exists | Only the pre-warmed minimum; more is added after the reaction delay |
| Reaction to a bigger peak than estimated | **None automatically**; capacity is fixed, so ρ exceeds 1 and users see slow pages or errors until someone adds VMs (booting takes minutes) | Adds replicas within quotas and available node capacity; limited by the delay and by the maximum setting |
| Reaction delay | Not applicable (but human delay if the estimate was wrong) | Metric collection and autoscaler period (tens of seconds), pod start (seconds), and, if new nodes are needed, VM provisioning (minutes) |
| Steady response time when estimate is right | Good and stable | Good once scaled; may be worse at the start of a very sharp peak unless pre-warmed |
| Failure effect | A failed VM removes `1/V_peak` of capacity; replacement takes minutes | Replacement pods start in seconds if node capacity exists |

**Assessment:** if the peak is accurately predicted and fixed VMs have headroom, they give the **most predictable** response time, because nothing depends on a reaction. Autoscaling gives better protection against **mis-estimation and uneven demand** but is reactive; it matches fixed VMs only if it is **pre-warmed** and the quotas allow growth. The database is the common bottleneck, so neither design gives a good response time if the database is undersized. Actual response times (for example 95th percentile latency) are to be measured under load in Part B3 and B4.

## 4. Resources left idle on the other 364 days

Assume, as in the case study, a few hundred users per day on ordinary days and, as a stated assumption from the phrase "one result day per semester" in B4, **two result days a year**.

- A fixed design is sized for the two-hour peak but runs for 8,760 hours a year. Peak hours are about `2 days × 2 h = 4 h`, which is roughly `4 / 8,760 ≈ 0.05%` of the year.
- For the rest of the year the load is a small fraction of the capacity. The average utilisation is therefore very low: `U_avg = (Σ load) / (V_peak · 8,760 · μ_vm)`, close to zero for this portal.
- The idle part is `V_peak · 8,760 · (1 − U_avg)` VM-hours of reserved CPU and memory (and the associated disk, licences and patching effort).

| Resource | Fixed VMs | Autoscaled pods |
|---|---|---|
| Idle CPU and memory on ordinary days | Almost the whole fleet (`V_peak − V_needed_today` VMs) | Only the small baseline (`V_min`) |
| Idle between result days | Paid for and powered on | Released by scale-down |
| Operating effort for idle machines | Patching, monitoring for all `V_peak` VMs | Patching of the baseline nodes; managed nodes may be replaced automatically |

A mitigation for fixed VMs is **scheduled start and stop** (turn VMs on before result day and off afterwards). This recovers most of the saving, but it needs a plan, depends on boot time and can be forgotten, and it is a manual form of the scaling that Kubernetes automates. Idle capacity on the university's **own** servers is a different case (sunk cost, but it blocks other workloads) and is analysed in Part B4.

## 5. Cost model

Let `p` be the price of one VM-hour (from the provider's current price list), `H = 8,760` hours per year, `h_peak` the number of hours per year that the larger fleet is actually needed (including ramp-up and ramp-down), and `V_min` the everyday baseline.

- Fixed, always on: `Cost_fixed = V_peak · H · p`
- Fixed with scheduled start/stop: `Cost_sched = V_min · H · p + (V_peak − V_min) · h_peak · p`
- Autoscaled: `Cost_auto = V_min · H · p + Σ (extra VMs × hours used) · p + management overhead`

Common to both: the managed database, load balancer and network charges. Additional costs in the Kubernetes design: the managed control plane fee (if the provider charges one) and the engineering effort for Kubernetes. Additional costs in the fixed design: the manual operations and the risk of an outage if capacity is wrong. Committed-use discounts can lower the price of the baseline in both cases; they are less useful for the rarely used peak capacity.

## 6. Illustration of the method (hypothetical parameters, not measurements)

Assume, only to show the calculation, `V_peak = 20`, `V_min = 2`, `H = 8,760`, and a total of `h_peak = 12` hours per year of larger capacity (two result days, six hours each including ramp). The price is normalised to `p = 1` unit per VM-hour.

| Design | VM-hours per year | Relative to fixed always-on |
|---|---|---|
| Fixed, always on | 20 × 8,760 = 175,200 | 100% |
| Fixed with scheduled start/stop | 2 × 8,760 + 18 × 12 = 17,736 | about 10% |
| Autoscaled (same extra hours) | 17,736 (plus any pre-warm hours and management overhead) | about 10%, plus Kubernetes costs |

The pattern is the point: because the peak is only a tiny fraction of the year, the compute cost of an always-on fixed fleet is roughly `V_peak / V_min` times larger than that of an elastic design. The real ratio depends on the values measured in Part B4.2 and on the prices taken from the provider.

## 7. Complexity and trade-off summary

| Quantity | Fixed VMs | Kubernetes with autoscaler |
|---|---|---|
| Compute cost per year | `O(V_peak · H)` | `O(V_min · H + extra · h_peak)` |
| Idle capacity | High, `O(V_peak · H)` | Low |
| Capacity added after the start | `O(minutes)` per VM, manual or scripted | Pods in seconds; nodes in minutes |
| Response-time risk | Under-estimated peak (no reaction) | Reaction delay, quotas, wrong requests/limits |
| Operational effort | Patch and manage `V_peak` VMs: `O(V_peak)` | Run the cluster and the autoscaler; patching of nodes; higher skills required |
| Predictability on result day | High if estimate correct | High if pre-warmed and tested |
| Recovery from instance failure | New VM boot: minutes | New pod: seconds |

## 8. Conclusion

Fixed VMs sized for result day would work and are simple and predictable on the day, but about 99.95% of the year the capacity is idle and still paid for, recovery is slower, and a wrong estimate cannot be corrected automatically. Kubernetes with an autoscaler, **pre-warmed before result day and tested**, keeps the day-to-day footprint small, scales with demand and replaces failed instances quickly, at the cost of more complexity and a need to measure requests, limits and database capacity. For this portal, the Kubernetes design is recommended, and the final comparison with the university's own servers and with a hybrid design is made in Part B4.
