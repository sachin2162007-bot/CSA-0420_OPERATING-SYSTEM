# Task 05: Distributed and Cloud-Based Storage Systems

**Course**: CSA04 – Operating Systems  
**Student Name**: S. Sachin  
**Register Number**: 192525045  
**Course Outcome**: CO4 (Bloom's Level K5 – Evaluate)  
**Institution**: SIMATS Engineering  

---

## 1. Overview and Problem Context
This task designs and evaluates an enterprise distributed storage architecture for a university distributed across 5 regional campuses. The project assesses centralized Network File System (NFS), Hadoop Distributed File System (HDFS), Ceph unified storage, and public cloud object stores (AWS S3, Google Cloud Storage) across durability, consistency, scalability, fault tolerance, and total cost of ownership (TCO).

---

## 2. Directory Structure and Files
- `storage_architecture_comparison.md`: 8-dimension comparative matrix covering NFS, HDFS, Ceph, AWS S3, and GCS.
- `replication_consistency.md`: Formal analysis of 3-copy replication, rack/campus-aware topologies, CAP theorem (CP vs AP), and CRC32C bit-rot repair.
- `device_management.md`: Mathematical derivation of RAID 5 rebuild failure on modern drives and evaluation of RAID 6, RAID 10, hot spares, and Ceph BlueStore.
- `nfs_alternative_analysis.md`: Detailed critique of single NFS server bottlenecks, WAN latency chokes, split-brain risks, and client D-state hangs.
- `complexity_analysis.md`: Algorithmic analysis of CRUSH ($O(\log P)$ zero-lookup placement) vs NameNode memory limits ($O(B)$).
- `storage_architecture.yaml`: Production-grade Ceph cluster deployment manifest spanning 5 campuses.
- `expected_output.txt`: Verified validation log of multi-campus failover and data scrub simulation.

---

## 3. Key Findings and Justified Architecture
1. **Rejection of Single NFS Server**: A single NFS server at Campus A creates a catastrophic single point of failure, suffers from high WAN latency on directory lookups, and leads to client workstation freezes under network partitions.
2. **Adoption of Multi-Campus Ceph Cluster**:
   - Eliminates metadata lookup bottlenecks via the deterministic CRUSH algorithm.
   - Enforces cross-campus fault domains (3-way replication across Campus A, B, and C).
   - Provides local edge caching at Campuses D and E to deliver sub-millisecond student read latencies.
   - Provides unified storage: CephFS for Linux labs, RBD for virtual machines, and S3-compatible RGW for campus video streaming.

<!-- Add Gemini screenshot here: Ceph multi-campus architecture and CRUSH map topology -->
