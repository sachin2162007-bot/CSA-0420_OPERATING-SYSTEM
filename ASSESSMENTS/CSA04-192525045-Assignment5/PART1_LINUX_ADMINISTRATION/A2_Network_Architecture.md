# Task A2.2 – Networking Services (2 Marks)

All addresses and names below are **design placeholders** (private RFC 1918 range and the reserved example domain). The university must substitute its real values. See [Diagram D7](diagrams/network_topology.md) and [Diagram D8](diagrams/request_flow.md).

## 1. IP Addressing

| Segment | Subnet | Host | Address | Role |
|---|---|---|---|---|
| DMZ | 10.20.10.0/24 | `portal-web` | 10.20.10.10 | Nginx + Gunicorn |
| DMZ | | gateway | 10.20.10.1 | Default route to edge firewall |
| Internal | 10.20.20.0/24 | `portal-db` | 10.20.20.10 | PostgreSQL |
| Internal | | `backup` | 10.20.20.20 | Backup storage |
| Internal | | `dns-int` | 10.20.20.2 | Internal DNS resolver |
| Management | admin workstations | | allow-listed addresses | SSH source |

Design decisions:
- **Static IP addresses** for servers (configured with netplan or NetworkManager), so DNS records and firewall rules do not change.
- **Private addresses** for all hosts; only the edge firewall/NAT exposes a public IP for ports 80 and 443.
- The **database is on a separate internal subnet** and is not reachable from the internet. A /24 subnet gives 254 usable hosts, ample for this deployment and for growth.
- Optional IPv6 can be added with the same segmentation.

## 2. DNS

| Record | Type | Value | Purpose |
|---|---|---|---|
| `results.example.edu` | A (and AAAA) | public IP of the edge firewall | Name students use |
| `portal-web.example.edu` | A | 10.20.10.10 | Internal name |
| `portal-db.example.edu` | A | 10.20.20.10 | Internal name used in the application configuration |

- **Internal DNS** resolves private names for servers; the **public DNS** zone publishes only `results.example.edu`.
- **TTL planning:** lower the TTL (for example to a few minutes) before result day so that the address can be changed quickly if a server must be replaced; raise it again afterwards.
- Servers use `/etc/resolv.conf` (or `systemd-resolved`) pointing at the internal resolver.
- DNS lookup cost for a client is one query, cached by resolvers; DNS can also distribute load across several front-end addresses (round-robin) if more servers are added later.

## 3. Nginx Reverse Proxy

Roles: terminate TLS, serve static files directly, protect the application, and forward dynamic requests to Gunicorn. Reasons: Nginx handles very many idle connections efficiently with an event-driven design, shields slow clients from the Python workers, and gives one place for TLS, limits and logging.

Illustrative configuration (design sketch, not tested):

```nginx
# /etc/nginx/sites-available/portal
limit_req_zone $binary_remote_addr zone=login:10m rate=5r/s;

upstream portal_app {
    server unix:/run/portal/portal.sock;
}

server {
    listen 80;
    server_name results.example.edu;
    return 301 https://$host$request_uri;          # force HTTPS
}

server {
    listen 443 ssl;
    server_name results.example.edu;
    ssl_certificate     /etc/ssl/portal/fullchain.pem;
    ssl_certificate_key /etc/ssl/portal/privkey.pem;

    location /static/ { alias /srv/portal/static/; expires 1h; }

    location /login   { limit_req zone=login burst=10; proxy_pass http://portal_app; }

    location / {
        proxy_pass http://portal_app;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_read_timeout 30s;
    }
}
```

Points: HTTP redirected to HTTPS; certificates stored with restricted permissions; request-rate limiting on `/login` reduces abuse; `proxy_set_header` passes the real client address for logging; Nginx can also later balance across several Gunicorn back ends using `upstream`.

## 4. Firewall Rules (ufw)

Policy: **deny all incoming by default; allow only what is needed.** `ufw` is a front end to netfilter/iptables.

`portal-web` (DMZ):
```bash
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow 80/tcp                     # HTTP (redirect)
sudo ufw allow 443/tcp                    # HTTPS
sudo ufw allow from 10.20.30.0/24 to any port 22 proto tcp   # SSH, admin network only
sudo ufw limit 22/tcp                     # (alternative to above) rate-limit SSH attempts
sudo ufw enable
```

`portal-db` (internal):
```bash
sudo ufw default deny incoming
sudo ufw allow from 10.20.10.10 to any port 5432 proto tcp   # only the web server
sudo ufw allow from 10.20.30.0/24 to any port 22 proto tcp   # SSH, admin network only
sudo ufw enable
```

| Port | Protocol | Allowed from | Host |
|---|---|---|---|
| 80 | TCP | Anyone | `portal-web` |
| 443 | TCP | Anyone | `portal-web` |
| 22 | TCP | Admin network only | both |
| 5432 | TCP | `portal-web` only | `portal-db` |
| everything else | | Denied | both |

An equivalent `iptables` rule set would use `-P INPUT DROP`, a rule accepting established/related connections, and `-A INPUT -p tcp --dport 443 -j ACCEPT`. `ufw` is chosen because rules are easier to read and audit. The edge firewall repeats the policy (defence in depth). Rule matching is sequential, O(r) for r rules, which is negligible for a short list.

## 5. SSH Access Using Keys

Process:
1. Each administrator creates a key pair on their own workstation: `ssh-keygen -t ed25519`; the private key is protected by a passphrase and never leaves the workstation.
2. The **public key** is placed in `~/.ssh/authorized_keys` on the server (`ssh-copy-id`), with permissions `700` on `.ssh` and `600` on the file.
3. The server accepts only keys. Hardened `sshd_config` settings:

```text
PermitRootLogin no
PasswordAuthentication no
PubkeyAuthentication yes
KbdInteractiveAuthentication no
AllowGroups ssh-users
MaxAuthTries 3
LoginGraceTime 30
X11Forwarding no
```

4. Port 22 is reachable only from the admin network (firewall rule above). Failed attempts are visible in the authentication log and reviewed (see [A2_User_Service_Log_Design.md](A2_User_Service_Log_Design.md)).
5. Leaving staff are removed by deleting their key and account.

Why keys: a key cannot be guessed like a password, brute-force attacks fail, and every login is tied to a named person.
