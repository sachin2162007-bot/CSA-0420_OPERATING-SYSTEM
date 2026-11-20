# Stage 5: Architecture Evaluation and Cloud/CDN Alternative

## 1. On-Premises Hybrid Architecture vs. Public Cloud (AWS S3 + CloudFront)

| Architectural Metric | On-Premises Hybrid Storage Cluster | Public Cloud Solution (AWS S3 + CloudFront CDN) |
| :--- | :--- | :--- |
| **Initial Hardware CapEx** | $\mathbf{\$85,000}$ (Servers, NVMe arrays, 100GbE switches) | $\mathbf{\$0}$ (Zero upfront capital expenditure) |
| **Monthly Storage Cost** | $\$300$ (Server chassis power & cooling) | $\$552$ (24 TB S3 Standard @ $\$0.023/\text{GB}$) |
| **Peak Bandwidth Egress** | **Included** (University NKN / Dark Fiber backbone) | **CATASTROPHIC!** (100 Gbps peak egress) |
| **Monthly Egress Cost** | $\$2,500$ (Fixed 100G transit port commitment) | $\mathbf{\$648,000\text{ / month}}$ ($10.8\text{ PB}$ egress @ $\$0.06/\text{GB}$) |
| **3-Year Total Cost of Ownership (TCO)** | $\mathbf{\$175,000 - \$220,000}$ | $\mathbf{\$23,340,000}$ |
| **Latency to Campus Workstations** | Sub-millisecond ($<1\text{ ms}$ over campus LAN) | $15\text{ ms} - 45\text{ ms}$ (Public Internet transit) |
| **Data Sovereignty & Privacy** | Complete on-premise governance; FERPA compliant | Bound to cloud provider terms and regional privacy laws |

---

## 2. In-Depth TCO Analysis and Financial Derivation

### Cloud Egress Calculation (The Financial Killer)
- Concurrency: $25,000$ active streams @ $4.0\text{ Mbps} = 12.5\text{ GB/s}$.
- Assuming peak streaming occurs 8 hours per day during academic semesters (25 days/month):
  $$\text{Daily Egress} = 12.5\text{ GB/s} \times 3,600\text{ s/hr} \times 8\text{ hrs} = 360,000\text{ GB} = 360\text{ TB/day}$$
  $$\text{Monthly Egress} = 360\text{ TB/day} \times 25\text{ days} = 9,000\text{ TB} = \mathbf{9.0\text{ PB/month}}$$
- AWS CloudFront data transfer out rate (Tier 1 blended: $\$0.06/\text{GB}$):
  $$\text{Monthly Cost}_{\text{Egress}} = 9,000 \times 1,000 \times \$0.06 = \mathbf{\$540,000\text{ to }\$648,000\text{ PER MONTH}}$$

### Final Recommendation
The public cloud solution is financially unviable for a university operating dedicated campus network backbones. The **On-Premises Hybrid Tiered Architecture** delivers superior streaming throughput, guaranteed deterministic latency, zero egress penalties, and achieves a **99.1% cost savings** over a 3-year operating lifecycle.
