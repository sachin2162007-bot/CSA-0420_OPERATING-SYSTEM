# B1.2 – Cron Scheduling

## The crontab actually installed (`crontab -l`, ACTUAL output)

```text
SHELL=/bin/bash
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
# m    h dom mon dow command
0      * *   *   *   /opt/portal-admin/Task01/log_analysis.sh >> /var/log/portal/log_analysis_cron.log 2>&1
*/10   * *   *   *   /opt/portal-admin/Task01/disk_space_alert.sh 85 >> /var/log/portal/disk_alert_cron.log 2>&1
```

| Field order | `m h dom mon dow command` |
|---|---|
| `0 * * * *` | minute 0 of **every hour**: the official requirement "run every hour" |
| `*/10 * * * *` | every 10 minutes: disk-space alert, same frequency as the Part A design (A2.3) |
| `>> file 2>&1` | append standard output **and** errors to a log, because cron has no terminal |
| `SHELL=`, `PATH=` | cron starts jobs with a minimal environment; the scripts use `awk`, `sort`, `df`, `logger` from the standard path |

Scripts were installed to `/opt/portal-admin/Task01/` (copied from this repository, `chmod +x`) and the crontab was loaded with `crontab /tmp/crontab.txt`. To reproduce:

```bash
sudo mkdir -p /opt/portal-admin /var/log/portal
sudo cp -r Task01 /opt/portal-admin/ && sudo chmod +x /opt/portal-admin/Task01/*.sh
sudo crontab -e        # paste the two job lines above (and the SHELL/PATH lines)
sudo crontab -l        # verify
```

## Evidence that cron really executes the script

The hourly line fires at minute 0, which could not be waited for inside the session, so a **temporary every-minute test line** with the identical command was added, observed, then removed (the final crontab above no longer contains it):

```cron
* * * * * /opt/portal-admin/Task01/log_analysis.sh >> /var/log/portal/cron_test.log 2>&1
```

ACTUAL content of `/var/log/portal/cron_test.log` after cron ran it at 06:32:01 (identical to the expected output):

```text
Requests per status code:
200 4
302 1
403 1
404 1
Busiest client:
10.0.0.5 4
Total bytes sent: 43200
```

## Status of this evidence
| Item | Status |
|---|---|
| `cron` daemon running and executing the script | **Done** (test line, above) |
| The exact hourly line `0 * * * *` firing at the top of an hour | **Pending** – installed and listed (`crontab -l`), but the session did not wait for the next full hour. `/var/log/portal/log_analysis_cron.log` will show the first run |
| The 10-minute disk-alert line firing | **Pending** – same reason; `/var/log/portal/disk_alert_cron.log` |
| Note | `cron` was started by hand (`cron` command) because this sandbox has no running systemd; on a normal server `systemctl enable --now cron` does this |

## Disk-space alert script, ACTUAL runs (`disk_space_alert.sh`)

```text
$ ./disk_space_alert.sh 
2026-11-21 06:29:45 OK: all file systems below 85%
exit status: 0
$ ./disk_space_alert.sh 1
2026-11-21 06:29:45 ALERT: / (/dev/vda) is 42% full (threshold 1%)
exit status: 1
$ ./disk_space_alert.sh abc
threshold must be a whole number (percent)
exit status: 2
```

Run 1: below the 85 % default → exit 0. Run 2: threshold forced to 1 % to prove that the alert path works → message printed (also sent to syslog by `logger`), exit 1. Run 3: invalid argument → exit 2. Exit codes let cron/monitoring distinguish "OK" from "alert".

Why `-x tmpfs -x devtmpfs -x squashfs -x overlay`: these pseudo or read-only file systems are always full or irrelevant and would cause false alerts.
