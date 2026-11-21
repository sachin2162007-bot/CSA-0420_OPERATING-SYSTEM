# Alternative analysis – public cloud only vs own servers + Kubernetes

**No cloud prices are used.** All costs are symbolic. p = price of one cloud instance-hour (instance sized like one measured container; unknown until a quote is obtained),
H = 8,760 hours/year, N = instances needed on result day (from `capacity_estimation.md`, e.g. 3–22 for k=10, depending on D), W = hours the result-day fleet runs
(assumption: 4 h = 2 h window + 1 h ramp-up + 1 h ramp-down), result days per year = 2 (one per semester), n0 = always-on minimum (assumption: 2 for availability), M = managed database/load-balancer/egress, billed per hour or per GB (unknown).

| Cost over one year | Formula (units of p) | Example, N = 21, p = 1 unit (illustrative, not a price) |
|---|---|---|
| Cloud, fixed fleet sized for result day | N x H x p + M | 21 x 8,760 = 183,960 units |
| Cloud, autoscaled (min n0, burst to N) | n0 x H x p + 2 x (N - n0) x W x p + M | 17,520 + 2 x 19 x 4 = 17,672 units |
| Own servers + Kubernetes | A + E + S  (A = yearly depreciation of servers already owned, E = electricity/cooling, S = staff time) | unknown: universities already own the servers, so the marginal cost is E + S; obtain from the data-centre office |

Reading: autoscaling in the cloud costs about 10x less than a fixed result-day fleet in this example only because 364 days run at n0; the 10x is a property of the formula
(N x H vs about n0 x H), independent of the price. Own servers win when A + E + S < the cloud formula's total for the real p; break-even p* = (A + E + S - M) / (n0 x H + 2 x (N - n0) x W).

| Criterion | Cloud only | Own servers + Kubernetes |
|---|---|---|
| Time to add capacity | Minutes (API/autoscaler), no hardware order | Fixed by installed hardware; beyond it, procurement takes weeks to months (not measured; typical order of magnitude) |
| Student-data control | Provider hosts the data; needs contract, region choice, encryption, audit; shared responsibility | University controls hardware, network and access; carries all security work itself |
| Result-day risk | Quota limits, bill spikes, internet dependency | Capacity ceiling of the server room; no external dependency |
