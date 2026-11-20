# Task 05: Distributed and Cloud-Based Storage Report

**Course**: CSA04 – Operating Systems (CO4)  
**Student**: S. Sachin (192525045)  
**Marks Allocated**: 7 Marks  

## 1. Distributed Storage Architecture Evaluation
Comparative matrix evaluating NFS, HDFS, Ceph, AWS S3, and GCS across architecture, scalability, latency, POSIX compliance, and cost.

## 2. Failure Modes of Single NFS Server
A centralized NFS server across 5 campuses creates a critical single point of failure, suffers high WAN RPC metadata latencies, and triggers uninterruptible client D-state freezes during network disconnects.

## 3. Ceph Multi-Campus Deployment
Proposes an on-premise Ceph cluster with CRUSH failure domains, 3-campus replication, edge caching at satellite campuses, and automated CRC32C scrubbing for bit-rot repair.
