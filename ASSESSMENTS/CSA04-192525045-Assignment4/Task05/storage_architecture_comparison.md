# Task 05: Distributed and Cloud-Based Storage Architecture Comparison

## 1. Executive Architectural Matrix

| Dimension | NFS (Network File System v4.2) | HDFS (Hadoop Distributed File System) | Ceph (CephFS & RADOS) | AWS S3 (Simple Storage Service) | Google Cloud Storage (GCS) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Architecture Model** | Centralized Client-Server (Stateful/Stateless RPC) | Master-Worker (`NameNode` metadata + `DataNode` storage) | Fully Distributed, Decentralized (`CRUSH` algorithmic placement) | Globally Distributed Object Store (Multi-tenant REST API) | Globally Distributed Object Store (Colossus/Spanner backend) |
| **Primary Use Case** | Small-to-medium enterprise file sharing, home dirs | High-throughput batch analytics (MapReduce, Spark) | Unified enterprise storage (Block, Object, File) | Cloud-native applications, massive data lakes, archival | Cloud-native ML pipelines, BigQuery storage, global distribution |
| **Scalability** | Vertical (Limited to single filer/export host) | Horizontal ($100+\text{ PB}$, limited by NameNode RAM) | Horizontal (Exabyte-scale, millions of OSDs) | Virtually Infinite (Globally managed SaaS) | Virtually Infinite (Globally managed SaaS) |
| **Fault Tolerance** | Single Point of Failure (requires active/passive HA) | 3-way rack-aware replication or Reed-Solomon EC | Configurable replication ($N$-way) or Erasure Coding | 99.999999999% (11 9s) across $\ge 3$ Availability Zones | 99.999999999% (11 9s) across multi-region dual-buckets |
| **Read/Write Latency** | Low latency ($<1\text{ ms}$ on LAN) | High latency, massive sequential throughput | Low-to-medium ($1\text{ ms} - 5\text{ ms}$ on NVMe/SSD) | Medium latency ($50\text{ ms} - 150\text{ ms}$ REST HTTP/TLS) | Medium latency ($40\text{ ms} - 120\text{ ms}$ REST HTTP/TLS) |
| **POSIX Compliance** | Full POSIX (file locks, byte-range read/write) | Append-only; no arbitrary random writes | Full POSIX via CephFS POSIX client / kernel driver | Non-POSIX (Object CRUD: `GET`, `PUT`, `DELETE`) | Non-POSIX (Object CRUD: `GET`, `PUT`, `DELETE`) |
| **Consistency Model** | Close-to-open consistency (NFSv3) / Lease-based (NFSv4) | Strong consistency (Single-writer, append-only) | Strong consistency via primary OSD peering | Strong read-after-write consistency (since Dec 2020) | Strong global consistency (Spanner atomic timestamping) |
| **Cost Profile** | High storage hardware cost; zero API egress fees | Commodity server hardware; high operational management | Open-source software on commodity hardware; moderate OpEx | Pay-per-GB-month + request fees + severe egress fees | Pay-per-GB-month + operations + egress transfer fees |

---

## 2. In-Depth Architectural Evaluation

### 2.1 NFS (Network File System)
- **Strengths**: Trivial POSIX integration; transparent to existing Linux desktop workflows, user home directories, and lab compiler toolchains.
- **Critical Flaws for Multi-Campus Deployment**:
  - Over Wide Area Networks (WAN), NFS performance degrades exponentially due to synchronous RPC round-trips for file attributes (`getattr`).
  - Single point of failure: If the centralized NFS filer crashes, all remote mounts hang in uninterruptible sleep state (`D-state`), freezing student terminals across all 5 campuses.

### 2.2 HDFS (Hadoop Distributed File System)
- **Strengths**: Exceptional streaming throughput for huge sequential datasets; native rack awareness prevents loss during campus switch failure.
- **Critical Flaws**:
  - Strict append-only semantics make it impossible to host standard student home directories, SQLite databases, or random read/write assignments.
  - The centralized `NameNode` stores all block metadata in memory ($\approx 150\text{ bytes}$ per block), creating severe scaling bottlenecks if millions of small student code files ($<4\text{ KB}$) are stored.

### 2.3 Ceph (RADOS / CephFS)
- **Strengths**: The state-of-the-art open-source unified storage cluster. Uses the deterministic **CRUSH** (Controlled Replication Under Scalable Hashing) algorithm, completely eliminating centralized metadata bottlenecks. Clients compute the exact storage node (OSD) mathematically without consulting a central lookup server.
- **Verdict for 5-Campus University**: **Optimal on-premise foundation**. Ceph pools can be partitioned across campuses into separate CRUSH failure domains, providing seamless block devices (RBD) for virtual labs, CephFS for POSIX student directories, and Ceph RGW (S3-compatible) for video/media streaming.

### 2.4 Cloud Object Stores (AWS S3 & Google Cloud Storage)
- **Strengths**: Zero administrative overhead; 11 9s of durability; built-in edge caching via CloudFront / Google Cloud CDN.
- **Critical Flaws**:
  - High and unpredictable operational costs (OpEx): 200,000 students generating 100 Gbps of peak video streaming egress would incur catastrophic cloud egress bills ($\approx \$0.08/\text{GB}$, exceeding $\$200,000/\text{month}$).
