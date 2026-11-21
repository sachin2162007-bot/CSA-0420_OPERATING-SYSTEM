# Complexity Analysis (B1 and B2)

Notation: L = lines in a log, N = number of servers, R = distinct status codes, C = distinct client IPs, m = mounted file systems, k = layers, S = bytes of the image.

## 1. B1 log analysis (`log_analysis.sh`)
| Stage | Time | Space |
|---|---|---|
| `awk` field extraction (3 passes) | O(L) each | O(1) |
| `sort` of L values | O(L log L) | O(L) worst case (external merge sort spills to disk beyond memory) |
| `uniq -c` | O(L) | O(1) (works on sorted stream) |
| Second `sort` and `head` over the C counts | O(C log C) | O(C) |
| **Whole script** | **O(L log L)** | **O(L)** worst case, but the official sample is 7 lines |

An `awk` associative-array version (`count[$7]++`) would be O(L) time and O(R) space, because it avoids the sort of all L values; the assignment requires sort/uniq, so this is the alternative, not the implementation.

`disk_space_alert.sh`: O(m) time, O(1) space (one `df` pass).

### Measured scaling (MEASURED, 1 vCPU, `bench_log_analysis.sh`)
| Lines L | Log size | Time |
|---|---|---|
| 10,000 | 1 MB | 0.024 s |
| 100,000 | 6 MB | 0.117 s |
| 1,000,000 | 55 MB | 0.847 s |

Time grows slightly faster than linearly (×10 lines gives ×4.9 then ×7.2 time), consistent with O(L log L) plus fixed start-up costs; three points are not enough to fit the exponent exactly. Separate run on 1,000,000 lines: `grep -c` 0.063 s, status pipeline about 0.5 s, largest process peak RSS about 11.6 MB (**MEASURED**; the bench and this run differ because the bench runs three pipelines).

### Ten servers (MEASURED on one machine only)
Sequential 1.036 s vs parallel (`xargs -P 10`) 1.063 s for 10 × 100,000 lines. They are equal because the machine has **1 CPU**; this shows no speed-up and says nothing about real servers. **REASONED:** with N separate servers each analysing its own log in parallel, wall time is O(L log L) per server (plus network), total work O(N·L log L); centrally it is O(N·L log L) ingest work plus a roughly index-lookup query, so central logging trades continuous cost for fast queries.

## 2. B1 central logging (REASONED)
| Operation | Local scripts | Central (ELK-style index) |
|---|---|---|
| Find an error on N servers | O(N·L) scans (grep) | Index lookup, roughly O(log L_total + matches) per shard |
| Storage | O(L) per server | O(N·L) × index factor (PENDING measurement) |
| Memory | O(1) when idle | O(N·L) index structures partly in RAM; fixed JVM heap |

## 3. B2 images (REASONED; sizes PENDING)
| Quantity | Model |
|---|---|
| Image size | S = S_base + Σ layer sizes; only layers that change files add size, metadata layers add 0 |
| Pull time to a new server | T ≈ (compressed bytes not already present) / bandwidth: O(S) |
| Cold build time | Σ per-instruction time; with cache, a change in layer j rebuilds layers j..k only, O(k − j) |
| Ubuntu vs slim vs multi-stage | All O(S) in size and pull; they differ in constant S_base, in the number of OS packages p (vulnerability review effort O(p)), and multi-stage only reduces S by dropping build-stage content |
| Container limit enforcement | O(1) per scheduling period: cgroup quota check |

Measured values: **PENDING** (see `Task02/image_optimisation.md`).
