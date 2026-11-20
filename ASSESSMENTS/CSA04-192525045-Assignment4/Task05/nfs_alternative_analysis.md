# Single Centralized NFS Server vs. Replicated Distributed File System

## 1. Problem Scenario: 5-Campus University Deployment
The university administration proposes centralizing all student storage on a single high-performance NFS server located at Campus A, exported across high-speed WAN links to Campuses B, C, D, and E.

---

## 2. Critical Architectural Failure Modes of Single NFS Server

```
                 [Campus A: Primary Central NFS Filer]
                                   │
         ┌─────────────────┬───────┴─────────┬─────────────────┐
         │ (WAN Link 1)    │ (WAN Link 2)    │ (WAN Link 3)    │ (WAN Link 4)
         ▼                 ▼                 ▼                 ▼
    [Campus B]        [Campus C]        [Campus D]        [Campus E]
```

1. **WAN Latency and Synchronous Metadata Choke**:
   - NFSv3 and NFSv4 require synchronous round-trips for file lookups, permissions checks, and directory traversals (`readdirplus`).
   - A compiler run (`gcc main.c`) reading 50 header files over a $15\text{ ms}$ WAN link incurs $\ge 750\text{ ms}$ of network wait time alone, rendering remote lab workstations completely sluggish.
2. **Single Point of Complete University Failure (SPOF)**:
   - If the main power, core router, or storage backplane at Campus A fails, 100% of academic operations across all 5 campuses instantly halt.
3. **NFS Client Hangs (Uninterruptible D-State)**:
   - When the NFS server becomes unreachable, Linux clients with `hard` mounts freeze in uninterruptible kernel sleep (`TASK_UNINTERRUPTIBLE`). Standard commands (`ls`, `df`, `bash` tab-completion) lock up, requiring hard power cycles of lab workstations.
4. **Split-Brain and Network Partition Risks**:
   - If an active-passive HA pair is deployed at Campus A without strict fencing (STONITH), an intermittent heartbeat partition causes both filers to mount the shared storage simultaneously, corrupting filesystem superblocks irreversibly.

---

## 3. The Replicated Distributed File System Alternative (CephFS)

```
   [Campus A: 3x Mon, OSD Pool] ──WAN Quorum── [Campus B: OSD Pool]
                │                                    │
                ├──────────────WAN Quorum────────────┤
                │                                    │
   [Campus C: OSD Pool] ──────── WAN ───────── [Campus D & E: Edge Caches]
```

### Key Advantages
1. **Local Edge Read Caching**: Student read requests are serviced directly from on-campus storage nodes or local SSD caches at sub-millisecond speeds.
2. **Autonomous Campus Survivability**: Under wide-area network partitions, local campus operations continue uninterrupted. Students access local cached assignments and compile code locally.
3. **Algorithmic CRUSH Placement**: Data writes are deterministically replicated across campus failure domains without central controller bottlenecks.
4. **Unified Protocol Support**: Simultaneously provides POSIX filesystem exports for labs (`CephFS`), raw block devices for virtual machines (`RBD`), and S3 object storage (`RADOS Gateway`).

---

## 4. Final Strategic Recommendation
Abandon the single NFS server design immediately. Deploy an on-premise **Ceph Distributed Storage Cluster** spanning the 3 primary campuses with local proxy/caching gateways at Campuses D and E.
