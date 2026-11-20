# Task 1: Alternative Concept — Cloud Storage vs On-Premises File Server

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Trade-off Analysis: Google Drive / Microsoft OneDrive vs Managed Institutional Server  

---

## 1. Architectural Comparison
Instead of maintaining an on-premises Linux file server with local storage, an institution may outsource student file storage to enterprise cloud services such as Google Drive (Google Workspace for Education) or Microsoft OneDrive (Microsoft 365 Education).

| Comparison Dimension | On-Premises Managed Server (NFS/ext4/ZFS) | Enterprise Cloud Storage (Google Drive / OneDrive) |
| :--- | :--- | :--- |
| **Lookup & Access Time** | Sub-millisecond LAN latency (~0.5 ms to 2 ms); direct block/POSIX streaming | Dependent on Internet WAN bandwidth and round-trip time (30 ms to 150 ms latency) |
| **Storage & Deduplication** | Single authoritative storage pool; hard/soft links save space; zero duplicate copies | Tendency to create redundant user copies; synchronization clients download local duplicates |
| **Privacy & Data Sovereignty** | Absolute college control; data stays within campus firewall; zero third-party telemetry | Subject to Cloud Provider Terms of Service, jurisdiction laws (Cloud Act), and remote telemetry |
| **Administrative Control** | Full root control over quotas, file-system parameters, auditing, and backup cadence | Managed via administrative dashboard; limited low-level file-system customization |
| **Access Management** | Native LDAP/Kerberos/Active Directory integration; granular POSIX ACLs | OAuth 2.0 / SAML Single Sign-On (SSO); link-based or email-based access control |
| **Availability & Redundancy** | Bound to campus power, local UPS, and campus network uptime | Highly redundant multi-region cloud SLA (99.9% to 99.99% uptime); offline caching |
| **Collaboration** | Local git repositories, shared POSIX directories, and file locking | Real-time concurrent in-browser editing (Google Docs, Office 365) and shared folders |

---

## 2. Advantages and Disadvantages

### 2.1 On-Premises Server
- **Advantages:** Blazing LAN read/write throughput for compiling large codebases; total compliance with local data privacy regulations; zero monthly recurring SaaS subscription fees.
- **Disadvantages:** Demands dedicated IT personnel for hardware maintenance, drive replacement, backup tape rotations, and firewall security patching.

### 2.2 Cloud Storage Solution
- **Advantages:** Anywhere accessibility from mobile devices and home networks; high availability with geo-distributed disaster replication; native real-time collaborative editing.
- **Disadvantages:** High network egress fees and recurring cloud license overhead; high latency for build tools compiling thousands of small C source files; loss of granular POSIX file attribute control.

---

## 3. Final Strategic Recommendation
**Hybrid Enterprise Model:**
- Maintain an **On-Premises Linux Storage Cluster (NFS over ZFS)** for laboratory compile workloads, high-performance computing, automated assignment testing, and official grading.
- Integrate **Enterprise Cloud Storage (Google Drive / OneDrive)** via Institutional Single Sign-On (SSO) for student lecture notes, general document collaboration, and off-campus remote access.
