# Task 01 – B1 Administer the Linux Server (7 marks)

| Name | S. Sachin |
|---|---|
| Register number | 192525045 |
| Course | CSA04 Operating Systems, B.Tech AI and ML, SIMATS Engineering |
| Shell / tools used | GNU bash 5.2.21, mawk 1.3.4, GNU coreutils 9.4 (`sort`, `uniq`), cron 3.0pl1, nginx 1.24.0, ufw 0.36.2, sysstat 12.6.1 |
| Linux used | Ubuntu 24.04.5 LTS, kernel 6.18.44, micro-VM with 1 vCPU / 3992 MiB RAM (not a physical server; systemd is **not** PID 1) |

## Files
| File | Purpose | Evidence status |
|---|---|---|
| `log_analysis.sh` | Official page-46 script with TODO 1 and TODO 2 completed | **ACTUAL**: output identical to expected |
| `expected_output.txt` | Expected output copied from the official PDF | EXAMPLE (from PDF) |
| `actual_output.txt` | Output of `bash log_analysis.sh` in this session | ACTUAL |
| `disk_space_alert.sh` | Disk-space alert, threshold argument, exit codes | ACTUAL runs in `cron_configuration.md` |
| `cron_configuration.md` | Crontab used, cron execution evidence | ACTUAL (hourly firing PENDING) |
| `system_observation.md` | B1.1: `ps top free vmstat iostat df ss` outputs and explanation | ACTUAL |
| `systemd/portal.service` | systemd unit for Gunicorn | Syntax verified; not started (**PENDING**, no systemd) |
| `nginx/portal.conf` | Nginx reverse proxy | ACTUAL (`nginx -t` and requests) |
| `ufw_rules.sh` | Firewall: only 22, 80, 443 | ACTUAL (enabled, block test from separate namespace) |
| `bench_log_analysis.sh` | Scaling benchmark used in the complexity analysis | ACTUAL |

## How to run
```bash
bash Task01/log_analysis.sh                      # compare with expected_output.txt
diff <(bash Task01/log_analysis.sh) Task01/expected_output.txt && echo PASS
bash Task01/disk_space_alert.sh 85               # 1 to 100 = threshold in percent
bash Task01/bench_log_analysis.sh                # about a minute
```
Service, Nginx, firewall and cron steps with exact commands and output: `../REPORT/B1_Implementation_Report.md`.

## Result of the task
`log_analysis.sh` reproduces the official expected output exactly. The portal ran behind Nginx under an unprivileged `portal` user with a default-deny firewall that was shown to block a non-allowed port. The systemd start-up and the real hourly cron firing are pending (no systemd in the sandbox / time).

## Git commit suggestion
`B1.1 observation`, `B1.2 log_analysis TODO 1-2`, `B1.2 disk alert + cron`, `B1.3 systemd + nginx + ufw`, `B1.4 comparison`.
