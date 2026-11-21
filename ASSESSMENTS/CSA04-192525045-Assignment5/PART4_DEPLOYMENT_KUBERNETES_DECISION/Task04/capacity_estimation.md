# B4.2 – Capacity estimate for 150,000 students in two hours

**Time available:** 2 x 60 x 60 = 7,200 s.  **Measured:** one container (`docker run`, 1 gunicorn worker, 1 vCPU host) = **2,005.6 requests/s**
(mean of 2,008.87 / 2,001.57 / 2,006.43; `ab -n 5000 -c 50 /results`). Kubernetes, 3 pods on the same single vCPU: about 2,555 rps (no gain, one core).

## Method
1. Average rate  R_avg = (students x requests per student) / 7,200 s
2. Peak rate  R_peak = R_avg x P  (P = peak-to-average factor; students log in in bursts)
3. Per-container usable rate  C = measured rps x u  (u = target utilisation, for headroom)
4. Containers needed  N = ceil(R_peak / C);  add 1 spare (N+1) so one failure does not break capacity.

## Assumptions (NOT measurements – change them and recompute)
* k = requests per student session: 1 (login only), 10, 30 (page, results JSON, static assets).
* P = 10 (result-day bursts); u = 0.5.
* D = database slowdown factor: the measured app serves sample rows with no database. D = 1 is the measured value; D = 10 means a real DB-backed request costs 10x the CPU (assumption, must be measured with Part 3's Compose stack).
* One container = the measured 1-worker container; a container capped at 0.5 CPU as in Part 3 Compose would give less – measure it before relying on it.

| k | R_avg (req/s) | R_peak = x10 | N, D=1 (C=1,003) | N, D=10 (C=100) | N+1 spare (D=1 / D=10) |
|---|---|---|---|---|---|
| 1 | 20.8 | 208.3 | 1 | 3 | 2 / 4 |
| 10 | 208.3 | 2,083.3 | 3 | 21 | 4 / 22 |
| 30 | 625.0 | 6,250.0 | 7 | 63 | 8 / 64 |

Worked example (k = 10, D = 1): R_avg = 150,000 x 10 / 7,200 = 208.3 req/s; R_peak = 2,083.3; C = 2,005.6 x 0.5 = 1,002.8; N = ceil(2,083.3 / 1,002.8) = 3 containers (+1 spare = 4).
Even without any peak factor and with k = 1, the average of 20.8 req/s is about 1% of one measured container: the sizing is driven by the burst factor and the database, not by the 7,200 s average.

## How to finish with your own numbers
Re-run `benchmark.sh container` (and the VM row), replace 2,005.6 by your rps, measure the Compose portal+PostgreSQL stack to obtain D, and recompute N with the formula above. Number of VMs is computed the same way using the VM's measured rps.
