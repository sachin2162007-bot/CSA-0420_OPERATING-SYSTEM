# Task 06: High-Concurrency Video Streaming Storage Architecture

## 1. System Requirements and Engineering Objectives
- **Target Audience**: 200,000 university students across multiple campuses.
- **Concurrent Streaming Workload**: Peak concurrency of **25,000 simultaneous streams**.
- **Media Profile**: High Definition (1080p, H.264/AAC), encoded at **4.0 Mbps** ($0.5\text{ MB/s}$).
- **Video Catalog**: 15,000 instructional video lectures, ranging from 300 MB to 2 GB (mean = 800 MB).
- **Core Engineering Challenge**: Delivering **100 Gbps (12.5 GB/s)** of sustained sequential-read throughput with bounded segment latency ($<200\text{ ms}$) without exhausting disk I/O budgets or incurring unsustainable cloud egress bills.

---

## 2. Five-Stage Architectural Framework

```
[Stage 1: Workload Characterization]
  │ (Zipfian Skew α=0.85, 25k Streams, 100 Gbps Read Throughput)
  ▼
[Stage 2: Capacity & Throughput Sizing]
  │ (12 TB Catalog Base -> 36 TB Raw Protection, 12.5 GB/s Bus Demand)
  ▼
[Stage 3: Multi-Tier Storage Architecture]
  │ (Tier 0 NVMe Cache -> Tier 1 SATA SSD -> Tier 2 HDD RAID 6 Array)
  ▼
[Stage 4: Simulation & Benchmark Validation]
  │ (All-HDD Saturation vs Hybrid Tiering Benchmark)
  ▼
[Stage 5: TCO & Cloud CDN Alternative Evaluation]
  │ (On-Premises Hybrid CapEx $85k vs Cloud Egress $648k/month)
```
