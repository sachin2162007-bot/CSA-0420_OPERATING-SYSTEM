# Diagram D7 – Network Topology (supports A2.2)

Addresses are **design assumptions** using the private range 10.20.0.0/16 and the documentation domain `example.edu`; replace with the university's real values.

```mermaid
flowchart TB
    NET["Internet clients<br/>students"] --> DNS["Public DNS<br/>results.example.edu -> public IP"]
    NET --> FW["Edge firewall / NAT<br/>443 and 80 only"]
    ADM["Admin workstation"] -->|"SSH key, 22, allow-listed"| WEB
    FW --> WEB
    subgraph DMZ["DMZ VLAN 10.20.10.0/24"]
        WEB["portal-web 10.20.10.10<br/>Nginx + Gunicorn<br/>ufw: 80, 443, 22"]
    end
    subgraph INT["Internal VLAN 10.20.20.0/24"]
        DB["portal-db 10.20.20.10<br/>PostgreSQL 5432<br/>ufw: only from 10.20.10.10"]
        BK["backup target 10.20.20.20"]
    end
    WEB -->|"5432"| DB
    DB -->|"nightly backup"| BK
    IDNS["Internal DNS 10.20.20.2"] -.-> WEB
    IDNS -.-> DB
```
