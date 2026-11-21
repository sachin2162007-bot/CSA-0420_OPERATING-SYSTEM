# Task A2.1 – Users, Services and Logs (2 Marks)

Design for the portal server(s) described in the case study. Names, paths and thresholds are **design choices**, not measured results. They are implemented and checked in Part B.

## 1. User Accounts

Principle: **least privilege, individual accountability, no shared logins.**

| Account | Type | Shell / login | Purpose |
|---|---|---|---|
| `alice`, `bob` (named admins) | Human | Bash, SSH key only | System administrators; use `sudo`, never log in as root |
| `ops1` (example) | Human | Bash, SSH key only | Operator with limited sudo (restart/status of portal) |
| `deploy` | Human or CI | SSH key only | Updates application code in `/srv/portal` |
| `portal` | System (`useradd -r`) | `/usr/sbin/nologin` | Runs Gunicorn and the Python application |
| `www-data` | System | nologin | Runs Nginx workers |
| `postgres` | System | nologin | Owns and runs PostgreSQL |
| `backup` | System | nologin (cron runs as root or via sudo rule) | Owns backup files |
| `root` | Superuser | SSH login disabled; password locked for remote use | Used only through `sudo` |

Account policy: password login over SSH disabled, system accounts have no shell, accounts are removed or locked when staff leave, and `/etc/passwd`, `/etc/group` and sudo changes are reviewed.

## 2. User Groups

| Group | Members | Rights |
|---|---|---|
| `sysadmins` | `alice`, `bob` | Full `sudo` (`%sysadmins ALL=(ALL) ALL`) |
| `operators` | `ops1` | `sudo` only for `systemctl status/restart portal nginx`, `journalctl` |
| `portal-app` | `portal`, `deploy`, `www-data` | Read access to application files; secrets file readable by group |
| `adm` | `alice`, `bob`, `ops1` | Read access to logs in `/var/log` (standard on Debian/Ubuntu) |
| `ssh-users` | all human accounts | Only these may log in (`AllowGroups ssh-users`) |

Resulting file ownership (see also [A1_Filesystem_Permissions.md](A1_Filesystem_Permissions.md)):

| Path | Owner:Group | Mode |
|---|---|---|
| `/srv/portal/` | `deploy:portal-app` | 750 (setgid on directories: 2750) |
| `/etc/portal/portal.env` | `root:portal-app` | 640 |
| `/var/log/portal/` | `portal:adm` | 750 |
| `/var/lib/postgresql/` | `postgres:postgres` | 700 |
| `/var/backups/portal/` | `root:root` | 700 |

## 3. systemd Services

systemd starts services at boot, supervises them, restarts them on failure, applies resource limits through cgroups, and collects their output in the journal.

| Unit | Role | Depends on | Restart policy |
|---|---|---|---|
| `nginx.service` | Reverse proxy, TLS, static files | `network-online.target` | `on-failure` |
| `portal.service` | Gunicorn serving the Python application | `network.target`, database reachable | `on-failure` |
| `postgresql.service` | Database | `network.target` | `on-failure` |
| `cron.service` | Scheduled scripts | basic system | default |
| `ssh.service` | Administration | network | default |

Sketch of the application unit (illustrative design, not tested output):

```ini
# /etc/systemd/system/portal.service
[Unit]
Description=University Result Portal (Gunicorn)
After=network.target postgresql.service
Wants=postgresql.service

[Service]
User=portal
Group=portal-app
WorkingDirectory=/srv/portal
EnvironmentFile=/etc/portal/portal.env
ExecStart=/srv/portal/venv/bin/gunicorn --workers 4 --bind unix:/run/portal/portal.sock app:app
Restart=on-failure
RestartSec=3
RuntimeDirectory=portal
# resource limits (cgroups)
MemoryMax=2G
CPUWeight=200
LimitNOFILE=65535
# hardening
NoNewPrivileges=true
PrivateTmp=true
ProtectSystem=strict
ReadWritePaths=/var/log/portal
ProtectHome=true

[Install]
WantedBy=multi-user.target
```

Design reasoning: `Restart=on-failure` gives automatic recovery from crashes; `MemoryMax` stops one runaway service exhausting the host; `NoNewPrivileges`, `PrivateTmp` and `ProtectSystem` limit damage if the application is compromised; `LimitNOFILE` allows many simultaneous connections. Values (workers, 2G) are starting points to be tuned from load tests. Management commands: `systemctl enable --now portal`, `systemctl status portal`, `systemctl restart portal`.

## 4. Log Monitoring

| Log source | Location / command | What it shows |
|---|---|---|
| Journal (all services) | `journalctl -u portal -u nginx -u postgresql` | Service start/stop, application errors |
| Nginx access log | `/var/log/nginx/access.log` | Each request, status code, client IP |
| Nginx error log | `/var/log/nginx/error.log` | Proxy failures, upstream timeouts |
| Authentication log | `/var/log/auth.log` or `journalctl _COMM=sshd` | SSH logins, failed attempts, sudo use |
| PostgreSQL log | `/var/log/postgresql/` | Slow queries, connection errors |
| Kernel messages | `dmesg`, `journalctl -k` | OOM kills, disk and NIC errors |
| Firewall | `ufw` logging to kernel log | Blocked connections |

Monitoring approach (kept simple and script-based, matching the official scope):
1. **Retention:** journald size limited (`SystemMaxUse=`); text logs rotated by `logrotate` (see [A2_Automation_Plan.md](A2_Automation_Plan.md)).
2. **Hourly summary script:** counts requests per HTTP status code and the busiest client IPs using `awk | sort | uniq -c`; this is the same technique as the Part B log-analysis task.
3. **Alert conditions** (initial thresholds to tune after load tests): HTTP 5xx rate above a set percentage of requests; many 401/403 responses from one IP; repeated failed SSH logins; any `Out of memory` kernel message; service restarted more than N times in an hour.
4. **Review routine:** administrators read the summary daily and watch `journalctl -f` and `top`/`vmstat` during result day.
5. **Security:** logs are readable only by `root` and the `adm` group, and are copied to the backup server so they survive a compromise of the web server.

Time to locate an error in logs on a single server is O(L) for L log lines (a linear `grep`); with rotation and dated files, the search can be limited to the relevant period.
