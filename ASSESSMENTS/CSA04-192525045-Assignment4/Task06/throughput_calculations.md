# Stage 2B: Throughput, Bus Bandwidth, and IOPS Sizing

## 1. Network and Bus Throughput Calculations
- Peak Active Streams: $25,000$
- Stream Bitrate: $4.0\text{ Mbps}$
- **Total Network Egress Throughput**:
  $$\text{Throughput}_{\text{net}} = 25,000 \times 4.0\text{ Mbps} = 100,000\text{ Mbps} = \mathbf{100.0\text{ Gbps}}$$
- **Total Storage Bus Read Throughput**:
  $$\text{Throughput}_{\text{storage}} = \frac{100.0\text{ Gbps}}{8} = \mathbf{12.5\text{ GB/s}}$$

---

## 2. IOPS Requirements Under HLS Chunking
In modern video streaming (HLS / MPEG-DASH), clients fetch discrete media chunks:
- Segment Size: $3.0\text{ MB}$ (representing $6.0$ seconds of video).
- **Chunk Read Request Rate (Aggregate IOPS)**:
  $$\text{IOPS}_{\text{chunk}} = \frac{25,000\text{ streams}}{6.0\text{ s}} = \mathbf{4,166.7\text{ IOPS}}$$
- While $4,167\text{ IOPS}$ appears modest, these are **large random $3.0\text{ MB}$ transfers**.

### Mechanical HDD Failure on Random Large Transfers
- An enterprise 7200 RPM HDD provides $\approx 150\text{ IOPS}$ max.
- Under random seeks, an HDD takes $8.5\text{ ms}$ seek $+ 4.16\text{ ms}$ rotational latency $+ (3.0\text{ MB} / 220\text{ MB/s} = 13.6\text{ ms}) \approx 26.3\text{ ms per chunk}$.
- Maximum chunks per second per HDD:
  $$\frac{1}{0.0263\text{ s}} \approx 38\text{ chunks/s} = 114\text{ MB/s}$$
- To serve $12.5\text{ GB/s}$ entirely from mechanical HDDs would require:
  $$\frac{12,500\text{ MB/s}}{114\text{ MB/s}} \approx \mathbf{110\text{ Enterprise HDDs spinning simultaneously!}}$$
- Such an array would draw excessive power, generate intense vibration, and still suffer seek jitter.
