# Task A2.3 – Automation (1 Mark)

Routine, repetitive and error-prone tasks should be scripted so they happen on time and the same way every time. Schedule: [Diagram D9](diagrams/automation_schedule.md).

## 1. Tasks to Automate

| # | Task | Why automate | Tool | Frequency |
|---|---|---|---|---|
| 1 | **Backups** of the database and configuration | Data loss is the highest risk; manual backups are forgotten | `backup.sh` (`pg_dump` plus `tar` of `/etc/portal`, `/etc/nginx`), run by cron | Daily at a quiet hour (for example 02:00) |
| 2 | **Backup verification and pruning** | An unverified backup may be useless; old copies fill the disk | Part of `backup.sh` (check size, test archive, delete beyond retention) | With each backup |
| 3 | **Log rotation** | Logs grow without limit and can fill the disk | `logrotate` (standard, run daily by a systemd timer or cron) with compression and retention | Daily |
| 4 | **Disk-space alerts** | A full disk stops the database and logs | `disk_alert.sh` using `df`, mail or syslog on threshold | Every 10 minutes |
| 5 | **Log summary** | Early detection of 5xx errors and abusive clients | `log_summary.sh` using `awk`, `sort`, `uniq` | Hourly |
| 6 | **Service health check** | Detect a dead service quickly | `systemctl is-active` and a `curl` to `/health` | Every few minutes |
| 7 | **Security updates check** | Keep the system patched | `apt` unattended upgrades or a reminder script | Daily check |

## 2. Example Cron Entries (design)

```cron
# m   h  dom mon dow  command
*/10  *  *   *   *    /usr/local/bin/disk_alert.sh
0     *  *   *   *    /usr/local/bin/log_summary.sh
0     2  *   *   *    /usr/local/bin/backup.sh
```

Alert logic of `disk_alert.sh` (pseudocode; the official SIMATS pseudocode standard is not included in the supplied PDF, so a generic structured style is used):

```text
ALGORITHM DiskAlert
INPUT:  threshold = 85 (percent)
OUTPUT: alert message when usage is too high
BEGIN
    FOR EACH mounted file system F IN df output
        usage <- percentage used of F
        IF usage >= threshold THEN
            SEND alert (F, usage, current date) to administrators
        END IF
    END FOR
END
```
Complexity: O(m) for m mounted file systems.

## 3. Design Rules for the Scripts

- Start with `set -euo pipefail` so errors are not ignored; use absolute paths because cron has a minimal environment.
- Write output to a log file and use the exit status so failures are visible.
- Use a lock (`flock`) so two runs cannot overlap, and store scripts in version control.
- Test restore from backup regularly; a backup that has never been restored is not proven.
- Keep scripts small and single-purpose.

## 4. Evaluation

Cron plus shell scripts are simple, need no extra software and are well suited to one or a few servers. Their weakness is that scripts and cron entries must be copied to each server, which motivates the Ansible comparison in [A2_Ansible_Alternative.md](A2_Ansible_Alternative.md).
