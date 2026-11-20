# Data Replication, Consistency, and the CAP Theorem

## 1. Multi-Copy Replication Mechanisms

### 1.1 HDFS Rack-Aware 3-Copy Policy
To balance write network performance against catastrophic rack/chassis switch failures, HDFS employs a strict topological placement policy:
1. **Replica 1**: Written to the local node hosting the client writer (or a randomly selected node if the client is external).
2. **Replica 2**: Written to an off-rack node in a distinct physical server rack.
3. **Replica 3**: Written to a different node residing in the **same remote rack** as Replica 2.

```
       [Client Writer]
              │ (Packet Stream)
              ▼
   [Campus A - Rack 1]             [Campus B - Rack 2]
   ┌─────────────────┐             ┌─────────────────┬─────────────────┐
   │   Node 1 (R1)   │───WAN/LAN──▶│   Node 2 (R2)   │────Intra-Rack──▶│   Node 3 (R3)   │
   └─────────────────┘             └─────────────────┴─────────────────┘
```

- **Failure Resilience**: Tolerates failure of any single node, an entire server rack, or the power supply feeding Rack 1.
- **Write Pipeline Optimization**: The client streams data packets to Node 1, which simultaneously pipelines bytes to Node 2, which pipelines to Node 3. Acknowledgments travel in reverse. Write latency is bounded by the slowest network link rather than three parallel client uploads.

### 1.2 Ceph CRUSH Rulesets and Failure Domains
Ceph avoids master lookup servers entirely. The cluster administrator defines a hierarchical map:
$$\text{root} \to \text{region} \to \text{datacenter/campus} \to \text{room} \to \text{row} \to \text{rack} \to \text{host} \to \text{osd}$$
A CRUSH rule enforces:
$$\text{select}(3, \text{type}=\text{campus})$$
This guarantees that for any object hash, the three replicas are deterministically assigned to three completely distinct physical campuses.

---

## 2. CAP Theorem Evaluation: CP vs. AP

According to Eric Brewer's CAP Theorem, under a network partition ($\mathbf{P}$), a distributed system must choose between:
- **Consistency ($\mathbf{C}$)**: Every read receives the most recent write or an error.
- **Availability ($\mathbf{A}$)**: Every non-failing node returns a non-error response, without guarantee of containing the latest write.

### 2.1 HDFS: Strict CP Architecture
- HDFS prioritizes absolute consistency. The `NameNode` acts as the single source of truth for file namespace and block leases.
- If a network partition isolates Campus D and Campus E from the primary `Active NameNode` at Campus A, nodes in D and E cannot allocate new blocks or modify files. Writes are rejected to prevent divergent file states.

### 2.2 Ceph: Strong Consistency with PG Peering (CP)
- Ceph uses primary-OSD peering. Writes to a Placement Group (PG) must be committed to all active acting-set OSDs before returning success to the client.
- If an OSD partition falls below minimum quorum (`min_size`), I/O is blocked until peering recovers, strictly preserving consistency.

### 2.3 Cloud Object Stores (AWS S3): High Availability (AP) transitioning to Strong Consistency
- Historically, Amazon S3 utilized eventual consistency (AP) for overwrite `PUT`s and `DELETE`s.
- In December 2020, AWS upgraded S3 to provide strong read-after-write consistency for all operations without sacrificing availability or latency, achieved via distributed timestamp tracking and synchronous replication across independent availability zones.

---

## 3. Data Integrity: Checksumming, Bit Rot, and Read Repair

1. **Silent Data Corruption (Bit Rot)**: Modern high-density storage drives suffer from undetected magnetic flux flips and flash gate charge leakage (Unrecoverable Read Error rate of $1\text{ in }10^{14}\text{ to }10^{15}\text{ bits}$).
2. **End-to-End CRC32C Checksums**:
   - During block writes, checksums are calculated for every $512\text{ bytes}$ or $4\text{ KB}$ chunk and stored in out-of-band metadata.
3. **Background Scrubbing**:
   - **Shallow Scrub (Daily)**: Compares object metadata, replica counts, and size tags across peer OSDs.
   - **Deep Scrub (Weekly)**: Reads all underlying data blocks from disk, recalculates cryptographic CRC32C checksums, and compares results bit-for-bit.
4. **Autonomous Read Repair**:
   - When a client reads a corrupted block, the mismatch is detected immediately. The primary node fetches the authentic replica from a healthy peer, services the client request transparently, and automatically overwrites the corrupt disk sector.
