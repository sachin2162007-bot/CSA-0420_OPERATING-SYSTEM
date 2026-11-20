# Stage 3: Multi-Tier Storage Architecture Design

## 1. Tiered Storage Topology

```
                       [200,000 Student Clients Across 5 Campuses]
                                           │
                                           ▼ (Dual 100GbE Uplinks)
                       ┌────────────────────────────────────────┐
                       │  Edge Load Balancer & Nginx Web Server │
                       │   (Linux Kernel eBPF + HLS Manifest)   │
                       └────────────────────────────────────────┘
                                           │
                 ┌─────────────────────────┴─────────────────────────┐
                 │ (83.7% Cache Hits)                                │ (16.3% Cold Misses)
                 ▼                                                   ▼
┌─────────────────────────────────┐                 ┌─────────────────────────────────┐
│     TIER 0 / TIER 1 NVMe CACHE   │                 │     TIER 2 CAPACITY POOL        │
│  - 4x 1.92TB Enterprise NVMe    │                 │  - 8x 16TB Enterprise 7200 HDDs │
│  - RAID 10 (3.84TB Usable)      │                 │  - RAID 6 Dual Parity (96TB)    │
│  - 14.0 GB/s Read Throughput    │                 │  - 1.5 GB/s Read Throughput     │
│  - 800,000 Random IOPS          │                 │  - 1,200 IOPS                   │
│  - Sub-millisecond Seek Latency │                 │  - Linux 'bfq' Scheduler        │
└─────────────────────────────────┘                 └─────────────────────────────────┘
```

---

## 2. Component Specifications

### 2.1 Tier 0 / Tier 1 Flash Storage (Fast Path)
- **Drives**: $4\times 1.92\text{ TB}$ PCIe Gen 4 Enterprise NVMe SSDs configured in RAID 10 ($3.84\text{ TB}$ usable).
- **Function**: Caches top 3,000 video lectures (83.7% of requests) + all HLS index manifests (`.m3u8`).
- **Performance**: $14.0\text{ GB/s}$ sustained sequential read; $800,000\text{ IOPS}$; $50\text{ }\mu\text{s}$ latency.
- **Linux I/O Scheduler**: `none` (direct multi-queue bypass).

### 2.2 Tier 2 Rotational HDD Storage (Capacity Path)
- **Drives**: $8\times 16\text{ TB}$ Seagate Exos Enterprise 7200 RPM SAS HDDs in RAID 6 ($96\text{ TB}$ usable).
- **Function**: Hosts complete 15,000 lecture library + master RAW video masters + backups.
- **Linux I/O Scheduler**: `bfq` (Budget Fair Queueing) to prevent seek starvation during cold access.

### 2.3 Filesystem Selection: OpenZFS on Linux
- **Adaptive Replacement Cache (ARC)**: $128\text{ GB}$ system DDR4 RAM acts as Tier -1 memory cache.
- **L2ARC (Level 2 Cache)**: NVMe SSD array acts as persistent secondary cache.
- **End-to-End Checksums**: Automatic fletcher4/SHA256 checksums on all blocks prevent silent bit rot.
