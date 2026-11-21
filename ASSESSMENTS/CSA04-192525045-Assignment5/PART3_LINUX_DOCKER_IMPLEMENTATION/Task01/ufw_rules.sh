#!/usr/bin/env bash
# Firewall for the portal server: deny all incoming, allow only SSH, HTTP and HTTPS.
# Run as root. WARNING: when logged in over SSH, keep the SSH rule BEFORE enabling the firewall.
# (Part A restricts SSH to the admin subnet; here SSH is open to anywhere because the sandbox has
#  no admin network. In production use: ufw allow from <ADMIN_SUBNET> to any port 22 proto tcp)
set -euo pipefail
ufw default deny incoming
ufw default allow outgoing
ufw allow 22/tcp     # SSH
ufw allow 80/tcp     # HTTP
ufw allow 443/tcp    # HTTPS
ufw --force enable
ufw status verbose
