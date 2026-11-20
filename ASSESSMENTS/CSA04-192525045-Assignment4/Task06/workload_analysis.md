# Stage 1: Workload Characterization and Traffic Modeling

## 1. Workload Parameters

| Parameter | Value | Technical Justification |
| :--- | :---: | :--- |
| **Total Registered Students** | 200,000 | Total enrolled student base across 5 campuses. |
| **Peak Active Concurrency** | 25,000 | 12.5% concurrency during exam periods and evening lecture hours. |
| **Stream Bitrate** | 4.0 Mbps | Standard 1080p30 H.264 high-profile video with 192 kbps AAC audio. |
| **Bandwidth Per Stream** | 0.5 MB/s | $(4.0 \times 10^6 \text{ bits/s}) / (8 \times 10^6 \text{ bits/MB})$. |
| **Peak Aggregate Throughput** | **100 Gbps (12.5 GB/s)** | $25,000 \times 4.0\text{ Mbps} = 100,000\text{ Mbps} = 100\text{ Gbps}$. |
| **Catalog Size** | 15,000 videos | University lecture repository across all academic departments. |
| **Video File Size** | 300 MB – 2 GB | Standard 45–90 minute lectures (mean = 800 MB). |
| **HLS Segment Duration** | 6.0 seconds | Apple HTTP Live Streaming (HLS) / MPEG-DASH chunking standard. |
| **Segment File Size** | 3.0 MB | $6.0\text{ s} \times 0.5\text{ MB/s} = 3.0\text{ MB per segment file}$. |
| **Chunk Request Rate** | 4,167 IOPS | $25,000 \text{ streams} / 6.0\text{ s} = 4,166.7 \text{ chunk requests/sec}$. |

---

## 2. Zipfian Popularity Distribution
Video on-demand accesses do not follow a uniform distribution. Accesses strictly follow a **Zipf-Mandelbrot Power Law** ($\alpha \approx 0.85$):
$$P(r) = \frac{1/r^\alpha}{\sum_{k=1}^N (1/k^\alpha)}$$
Where $r$ is video popularity rank ($1 \le r \le 15,000$).

### Key Architectural Implications
- **Top 5% of Videos (750 files = 600 GB)**: Account for **58.4% of all active streams** (14,600 concurrent users).
- **Top 20% of Videos (3,000 files = 2.4 TB)**: Account for **83.7% of all active streams** (20,925 concurrent users).
- **Cold Tail (Remaining 80% = 12,000 files)**: Accounts for only **16.3% of streams** (4,075 concurrent users).
- **Conclusion**: An ultra-fast SSD caching tier of only $2.4\text{ TB}$ can absorb over $83\%$ of all disk read I/O!
