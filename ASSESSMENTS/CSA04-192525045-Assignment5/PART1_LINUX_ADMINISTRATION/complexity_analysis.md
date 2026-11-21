# Consolidated Complexity Analysis – Part 1 (A1 and A2)

Big-O notation is used only where it is meaningful. Symbols: n = runnable tasks, d = path components, L = log lines, m = mounted file systems, r = firewall rules, N = servers, f = parallel connections. **No measurements are reported;** rows marked "measure in Part B" should be filled from real runs.

## 1. Operating-system mechanisms

| Mechanism | Time | Space | Note |
|---|---|---|---|
| CFS: pick next task | O(1) (leftmost node cached) | O(n) tree nodes | See A1.2 |
| CFS: enqueue / dequeue task | O(log n) | | Red-black tree |
| Page cache lookup | about O(1)–O(log n) per page | RAM proportional to cached pages | Reclaimable |
| Path lookup via dentry cache | O(d) | O(cached dentries) | A1.3 |
| Permission check | O(1) (three classes of bits) | O(1) per inode | |
| Firewall (ufw/netfilter) rule match | O(r) per packet | O(r) | Keep rule list short |
| `grep`/`awk` on a log | O(L) | O(1) streaming | |
| `sort` then `uniq -c` | O(L log L) | O(L) (may spill to disk) | A faster alternative is an `awk` associative array: O(L) time, O(k) space for k distinct keys |

## 2. Alternative: Windows Server vs Linux (A1)

| Quantity | Linux | Windows Server | Growth over N servers |
|---|---|---|---|
| Base memory per server | measure (`free -h`) | measure | O(N) total |
| Base disk per server | measure (`df -h`) | measure | O(N) total |
| Automation time | `T_write + N · t_run` | same form | O(N), or O(N/f) with parallel tools |
| Licence cost | no per-server fee | per-core and CAL licensing | O(N) |

## 3. Alternative: Shell scripts vs Ansible (A2)

| Quantity | Shell scripts | Ansible |
|---|---|---|
| Authoring effort | O(1) in N | O(1) in N, larger constant |
| Apply to N servers | O(N) | O(N / f) |
| Re-configure after change | O(N) manual | O(1) edit plus one run |
| Drift risk | grows with N | low |
| Space | negligible | control node + Python on targets |

Break-even: Ansible's larger setup constant is repaid once N is more than a few servers or changes become frequent.

## 4. Remaining step

Replace "measure" cells with values obtained in Part B and keep the commands used. Values quoted from vendors must be re-verified and cited in [references.md](references.md).
