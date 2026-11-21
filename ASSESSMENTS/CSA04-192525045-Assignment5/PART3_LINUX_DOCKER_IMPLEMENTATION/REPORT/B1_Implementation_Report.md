# B1 – Administer the Linux Server: Implementation Report (7 marks)

Student: S. Sachin | Register number: 192525045 | Course: CSA04 Operating Systems | B.Tech AI and ML, SIMATS Engineering

Evidence labels used throughout: **ACTUAL** = produced by a command run in this session (output pasted unedited); **PENDING** = not executed, command given; **EXAMPLE** = output copied from the official PDF as the expected result, never presented as measured.

## 0. Environment

```text
bash: GNU bash, version 5.2.21(1)-release (x86_64-pc-linux-gnu)
awk: mawk 1.3.4 20240123
mawk 1.3.4 20240123
sort: sort (GNU coreutils) 9.4
nginx: nginx version: nginx/1.24.0 (Ubuntu)
python: Python 3.12.3
gunicorn/flask/psycopg2: Flask           3.1.3 gunicorn        27.0.1 psycopg2-binary 2.9.13 
ufw: ufw 0.36.2
cron: Version: 3.0pl1-184ubuntu2
sysstat: sysstat version 12.6.1
docker: Docker version 29.1.3, build 29.1.3-0ubuntu3~24.04.2
compose: Docker Compose version 2.40.3+ds1-0ubuntu1~24.04.1
systemd: systemd 255 (255.4-1ubuntu8.18)
kernel: 6.18.44-fc-v726
cpus: 1, mem: 3992 MiB
```

Important limits of this environment, which decide what is ACTUAL and what is PENDING:
- It is a Firecracker micro-VM with 1 vCPU and about 4 GiB RAM, not a physical university server.
- PID 1 is `process_api`, **not systemd**, so `systemctl start/enable/status` cannot run here.
- The only reachable package sources are Ubuntu, PyPI, npm and GitHub; Docker Hub is not reachable (affects B2).

## 1. B1.1 – Observe the System (2 marks)
Full commands, actual outputs (baseline and under load) and explanation of every value: [`../Task01/system_observation.md`](../Task01/system_observation.md). Key findings: idle machine uses about 8 % of RAM, no swap; under load the single CPU saturates first (0 % idle, run queue 2–3) while memory and disk stay quiet; one Gunicorn worker costs about 30 MB; `ab` produced 8,884 sockets in TIME_WAIT.

## 2. B1.2 – Automate (2 marks)

### 2.1 Official log-analysis code (page 46)
File: `Task01/log_analysis.sh`. Only the two TODO lines were added; everything else is the original code.

TODO 1 (status codes, sorted by code):
```bash
awk '{print $7}' "$log" | sort -n | uniq -c | awk '{print $2, $1}'
```
`awk` extracts field 7, `sort -n` groups equal codes and orders them, `uniq -c` counts runs (`4 200`), the last `awk` swaps the columns into the required `200 4` form.

TODO 2 (busiest client):
```bash
awk '{print $1}' "$log" | sort | uniq -c | sort -k1,1nr -k2,2 | head -n 1 | awk '{print $2, $1}'
```
`sort -k1,1nr` puts the largest count first; `-k2,2` makes a tie deterministic (lowest IP first); `head -n 1` keeps the winner; the last `awk` prints `IP count`.

ACTUAL run with `bash log_analysis.sh`, compared with the official expected output using `diff` → **identical (PASS)**:

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

(Run on mawk 1.3.4 and GNU coreutils 9.4. Output files: `Task01/expected_output.txt` = official expected output; `Task01/actual_output.txt` = this run.)

### 2.2 Disk-space alert (`Task01/disk_space_alert.sh`) and cron
Actual runs of the alert script and the crontab (with the every-hour line `0 * * * * /opt/portal-admin/Task01/log_analysis.sh >> /var/log/portal/log_analysis_cron.log 2>&1`) are in [`../Task01/cron_configuration.md`](../Task01/cron_configuration.md). The cron daemon was shown executing the script (temporary one-minute test line, output identical to expected). The real hourly firing is **PENDING** (needs the clock to reach minute 0).

## 3. B1.3 – Services and Network (2 marks)

### 3.1 Application
`Task02/app.py` (small Flask app, routes `/`, `/results`, `/health`), installed in `/srv/portal` with a virtualenv, run by the unprivileged system user `portal` (`useradd -r -s /usr/sbin/nologin portal`).

ACTUAL: Gunicorn running as `portal` and answering directly:
```text
$ ps -o user,pid,cmd -C gunicorn
USER       PID CMD
portal    2262 /usr/bin/python3 /usr/local/bin/gunicorn --bind 127.0.0.1:8000 --workers 2 --access-logfile /var/log/portal/access.log --error-logfile /var/log/portal/error.log app:app
portal    2263 /usr/bin/python3 /usr/local/bin/gunicorn --bind 127.0.0.1:8000 --workers 2 --access-logfile /var/log/portal/access.log --error-logfile /var/log/portal/error.log app:app
portal    2264 /usr/bin/python3 /usr/local/bin/gunicorn --bind 127.0.0.1:8000 --workers 2 --access-logfile /var/log/portal/access.log --error-logfile /var/log/portal/error.log app:app
$ curl -si http://127.0.0.1:8000/health
HTTP/1.1 200 OK
Server: gunicorn
Date: Sat, 21 Nov 2026 06:30:28 GMT
Connection: close
Content-Type: application/json
Content-Length: 16

{"status":"ok"}

$ curl -s http://127.0.0.1:8000/results
{"results":[{"course":"CSA04","grade":"A","reg_no":"192525001"},{"course":"CSA04","grade":"B","reg_no":"192525002"},{"course":"CSA04","grade":"A","reg_no":"192525045"}],"source":"sample-data"}
$ curl -s http://127.0.0.1:8000/ | head -c 300
<!doctype html><title>University Result Portal</title><h1>University Result Portal</h1><p>Data source: sample-data</p><table border='1' cellpadding='6'><tr><th>Register No</th><th>Course</th><th>Grade</th></tr><tr><td>192525001</td><td>CSA04</td><td>A</td></tr><tr><td>192525002</td><td>CSA04</td><td
```

### 3.2 systemd service
File: `Task01/systemd/portal.service` (installed to `/etc/systemd/system/portal.service`). Content = Part A design (`Restart=on-failure`, `MemoryMax`, `NoNewPrivileges`, `PrivateTmp`, `ProtectSystem=strict`, `LimitNOFILE`) adapted to the sandbox (see section 5).

ACTUAL (syntax check only; `systemd-analyze verify` works without a running systemd):
```text
exit=0
(no warnings printed above = unit file syntax accepted)
```

**PENDING – not executed:** `systemctl daemon-reload`, `systemctl enable --now portal`, `systemctl status portal`, `systemctl kill -s KILL portal` (to show `Restart=on-failure`) cannot run because systemd is not PID 1 here. On a real server run:
```bash
sudo systemctl daemon-reload && sudo systemctl enable --now portal
systemctl status portal --no-pager
sudo pkill -9 -u portal gunicorn; sleep 4; systemctl status portal --no-pager   # should be active again
journalctl -u portal -n 20 --no-pager
```
To prove the service definition itself works, the **identical command line** from `ExecStart` was started by hand as user `portal` with `runuser` (this is what the application tests below used). The hardening options (`ProtectSystem=strict`, `PrivateTmp`, `MemoryMax`) were therefore **not** exercised.

During preparation, a real defect was found and fixed: `ProtectSystem=strict` makes `/srv/portal` read-only, but this Gunicorn version creates a control socket in `/srv/portal/.gunicorn/`. The option `--no-control-socket` was added to `ExecStart`. That the service then starts under `ProtectSystem=strict` is **not verified** (needs systemd).

### 3.3 Nginx reverse proxy
File: `Task01/nginx/portal.conf` → `/etc/nginx/sites-available/portal`, enabled by symlink; default site removed.

ACTUAL configuration test:
```text
$ nginx -t
nginx: the configuration file /etc/nginx/nginx.conf syntax is ok
nginx: configuration file /etc/nginx/nginx.conf test is successful
```

ACTUAL: Nginx (nginx/1.24.0) listening on port 80 and proxying to Gunicorn on 127.0.0.1:8000. Response headers show `Server: nginx`, access lines appear in both Nginx's and Gunicorn's logs (Gunicorn sees HTTP/1.0 because Nginx's default upstream protocol is 1.0 without `proxy_http_version 1.1`):
```text
$ ss -tlnp | grep -E ":80 |:8000 "
LISTEN 0      2048       127.0.0.1:8000      0.0.0.0:*    users:(("gunicorn",pid=2335,fd=5),("gunicorn",pid=2334,fd=5),("gunicorn",pid=2333,fd=5))
LISTEN 0      511          0.0.0.0:80        0.0.0.0:*    users:(("nginx",pid=2345,fd=7),("nginx",pid=2344,fd=7))                                 

$ curl -si http://127.0.0.1/health
HTTP/1.1 200 OK
Server: nginx/1.24.0 (Ubuntu)
Date: Sat, 21 Nov 2026 06:31:08 GMT
Content-Type: application/json
Content-Length: 16
Connection: keep-alive

{"status":"ok"}


$ curl -s http://127.0.0.1/results
{"results":[{"course":"CSA04","grade":"A","reg_no":"192525001"},{"course":"CSA04","grade":"B","reg_no":"192525002"},{"course":"CSA04","grade":"A","reg_no":"192525045"}],"source":"sample-data"}

$ curl -sI http://127.0.0.1/ | head -4
HTTP/1.1 200 OK
Server: nginx/1.24.0 (Ubuntu)
Date: Sat, 21 Nov 2026 06:31:08 GMT
Content-Type: text/html; charset=utf-8

$ tail -3 /var/log/nginx/portal_access.log
127.0.0.1 - - [21/Nov/2026:06:31:08 +0000] "GET /health HTTP/1.1" 200 16 "-" "curl/8.5.0"
127.0.0.1 - - [21/Nov/2026:06:31:08 +0000] "GET /results HTTP/1.1" 200 193 "-" "curl/8.5.0"
127.0.0.1 - - [21/Nov/2026:06:31:08 +0000] "HEAD / HTTP/1.1" 200 0 "-" "curl/8.5.0"
$ tail -3 /var/log/portal/access.log   (gunicorn log)
127.0.0.1 - - [21/Nov/2026:06:30:28 +0000] "GET / HTTP/1.1" 200 371 "-" "curl/8.5.0"
127.0.0.1 - - [21/Nov/2026:06:31:08 +0000] "GET /health HTTP/1.0" 200 16 "-" "curl/8.5.0"
127.0.0.1 - - [21/Nov/2026:06:31:08 +0000] "GET /results HTTP/1.0" 200 193 "-" "curl/8.5.0"
$ ps -o user,pid,cmd -C nginx; ps -o user,pid,cmd -C gunicorn (users)
portal    2333 gunicorn
portal    2334 gunicorn
portal    2335 gunicorn
root      2344 nginx
www-data  2345 nginx
```

Nginx was started directly (`nginx`), not through `systemctl`, for the same reason as above.

### 3.4 Firewall (ufw): allow only SSH, HTTP and HTTPS
Script: `Task01/ufw_rules.sh`. ACTUAL: ufw was really enabled in the sandbox:
```text
$ ufw default deny incoming
Default incoming policy changed to 'deny'
(be sure to update your rules accordingly)
$ ufw default allow outgoing
Default outgoing policy changed to 'allow'
(be sure to update your rules accordingly)
$ ufw allow 22/tcp
Rules updated
$ ufw allow 80/tcp
Rules updated
$ ufw allow 443/tcp
Rules updated
$ ufw --force enable
Firewall is active and enabled on system startup
$ ufw status verbose
Status: active
Logging: on (low)
Default: deny (incoming), allow (outgoing), deny (routed)
New profiles: skip

To                         Action      From
--                         ------      ----
22/tcp                     ALLOW IN    Anywhere                  
80/tcp                     ALLOW IN    Anywhere                  
443/tcp                    ALLOW IN    Anywhere                  

$ ufw status numbered
Status: active

     To                         Action      From
     --                         ------      ----
[ 1] 22/tcp                     ALLOW IN    Anywhere                  
[ 2] 80/tcp                     ALLOW IN    Anywhere                  
[ 3] 443/tcp                    ALLOW IN    Anywhere                  

$ curl -s -o /dev/null -w "%{http_code}
" http://127.0.0.1/health  (after enabling firewall)
200
$ iptables -S ufw-user-input
-N ufw-user-input
-A ufw-user-input -p tcp -m tcp --dport 22 -j ACCEPT
-A ufw-user-input -p tcp -m tcp --dport 80 -j ACCEPT
-A ufw-user-input -p tcp -m tcp --dport 443 -j ACCEPT
```

A request from the same machine over the loopback interface always passes, so on its own it proves nothing. To prove that the rules **block**, a separate network namespace (`ext`, address 10.99.0.2) was connected by a virtual cable to the host (10.99.0.1) and requests were made from "outside". Port 80 (allowed) worked; port 9999, where a listener was deliberately running on the host, **timed out** (packets dropped, curl exit 28):
```text
$ [from external namespace 10.99.0.2] curl http://10.99.0.1:80/health   (port 80, allowed)
HTTP 200
curl exit=0
$ [from external namespace] curl http://10.99.0.1:9999/   (port 9999, NOT allowed; a listener exists on the host)
HTTP 000
curl exit=28  (28 = timed out, packet dropped)
$ [from host loopback] curl http://127.0.0.1:9999/ (loopback is exempt)
HTTP 200
$ iptables -L ufw-user-input -n --line-numbers
Chain ufw-user-input (1 references)
num  target     prot opt source               destination         
1    ACCEPT     6    --  0.0.0.0/0            0.0.0.0/0            tcp dpt:22
2    ACCEPT     6    --  0.0.0.0/0            0.0.0.0/0            tcp dpt:80
3    ACCEPT     6    --  0.0.0.0/0            0.0.0.0/0            tcp dpt:443
```

Limitations: SSH is open to "anywhere" here; Part A restricts it to the admin subnet (`ufw allow from <admin subnet> to any port 22 proto tcp`). Port 443 is open in the firewall but **no HTTPS server block / certificate was configured**, so HTTPS service itself is **PENDING** (needs a real domain, for example with certbot). Docker, when running, adds its own iptables rules for published ports that bypass ufw; this matters for Task02.

## 4. Load test through the whole chain
ACTUAL: `ab -n 6000 -c 20 http://127.0.0.1/health` (client on the same single vCPU):
```text
This is ApacheBench, Version 2.3 <$Revision: 1903618 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 600 requests
Completed 1200 requests
Completed 1800 requests
Completed 2400 requests
Completed 3000 requests
Completed 3600 requests
Completed 4200 requests
Completed 4800 requests
Completed 5400 requests
Completed 6000 requests
Finished 6000 requests


Server Software:        nginx/1.24.0
Server Hostname:        127.0.0.1
Server Port:            80

Document Path:          /health
Document Length:        16 bytes

Concurrency Level:      20
Time taken for tests:   3.975 seconds
Complete requests:      6000
Failed requests:        0
Total transferred:      1044000 bytes
HTML transferred:       96000 bytes
Requests per second:    1509.52 [#/sec] (mean)
Time per request:       13.249 [ms] (mean)
Time per request:       0.662 [ms] (mean, across all concurrent requests)
Transfer rate:          256.50 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       1
Processing:     2   13   1.9     13      30
Waiting:        2   13   1.9     13      30
Total:          3   13   1.9     13      30

Percentage of the requests served within a certain time (ms)
  50%     13
  66%     13
  75%     14
  80%     14
  90%     16
  95%     17
  98%     19
  99%     20
 100%     30 (longest request)
```

This only shows that the chain Nginx→Gunicorn is functional and measures a trivial route; it is not the B3 benchmark.

## 5. B1.4 – Comparison with the Part A design (1 mark)

| Topic | Part A design (A2) | What was implemented | Reason for the change |
|---|---|---|---|
| Service account | `portal` system user, nologin, group `portal-app` shared with `deploy`/`www-data` | `portal` system user, nologin, own group `portal` | One-server demo; no `deploy` user or shared group needed |
| Admin accounts, sudo groups, `AllowGroups ssh-users` | Designed | **Not implemented** | Outside the B1 tasks; sandbox has no real admins |
| Gunicorn binding | Unix socket `/run/portal/portal.sock`, 4 workers | TCP `127.0.0.1:8000`, 2 workers | Port 8000 matches the Docker image (B2) and Kubernetes (B3); 2 workers for 1 vCPU. Loopback binding keeps it unreachable from outside |
| systemd unit | `EnvironmentFile`, `RuntimeDirectory`, `MemoryMax=2G`, depends on PostgreSQL | No env file/database dependency, `MemoryMax=512M`, plus `--no-control-socket` | Sample data instead of a database in B1; smaller limit for a 4 GiB VM; control-socket defect found (section 3.2) |
| systemd run-time proof | Expected to run | Unit verified but **not started** | No systemd in the sandbox (**pending**) |
| Nginx | HTTPS redirect, TLS certificates, `limit_req` on `/login`, upstream block | HTTP reverse proxy with `upstream`, forwarded headers | No domain or certificate; the rate-limit rule has no `/login` route to protect |
| Firewall | Default deny; 80, 443; SSH from admin subnet only | Default deny; 22, 80, 443; SSH from anywhere | No admin subnet in the sandbox |
| Log summary | `log_summary.sh`, hourly | `log_analysis.sh` (official code), hourly | Name and logic follow the official assignment |
| Disk alert | `disk_alert.sh`, every 10 min, mail/syslog | `disk_space_alert.sh`, every 10 min, stdout + syslog, exit code | No mail server |
| Cron script safety | `flock`, absolute paths, `set -euo pipefail` | Absolute paths and `set -euo pipefail` yes; **no `flock`** | Both scripts finish in milliseconds; overlap is unlikely, but `flock` would still be safer on a loaded server |
| Backups, logrotate | Designed | **Not implemented** | Not part of B1 |

What the implementation confirmed: the layered design works (Nginx in front, Gunicorn on loopback, default-deny firewall, unprivileged service user); the awk|sort|uniq technique named in A2.1 solves the log summary. What it changed: the control-socket/hardening interaction was not anticipated in Part A; and the observation that **CPU saturates first** supports sizing workers by cores, as the design assumed.

## 6. Alternative concept: central logging
See [`alternative_analysis.md`](alternative_analysis.md) (qualitative comparison with ELK/journald) and [`complexity_analysis.md`](complexity_analysis.md) (Big-O and measured data).

## 7. Pending evidence for B1
1. `systemctl enable --now portal`, `status`, restart-after-kill, `journalctl -u portal` (needs systemd).
2. Real hourly and 10-minute cron firings in their log files.
3. HTTPS with a valid certificate.
4. Run on a real Linux server or VM for server-realistic values in B1.1.
5. Screenshots (none were created; none are claimed).
