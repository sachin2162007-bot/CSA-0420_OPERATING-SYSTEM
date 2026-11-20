# Stage 2A: Storage Capacity and Sizing Calculations

## 1. Raw Storage Capacity Baseline
- Total Video Files: $N = 15,000$
- Average Video File Size: $\bar{S} = 800\text{ MB}$
- Base Usable Storage Requirement:
  $$\text{Capacity}_{\text{base}} = 15,000 \times 800\text{ MB} = 12,000,000\text{ MB} = \mathbf{12.0\text{ TB}}$$

---

## 2. Multi-Year Growth Sizing
- University curriculum expands at approximately $25\%$ compound annual growth rate (CAGR):
  $$\text{Capacity}_{\text{Year 1}} = 12.0\text{ TB} \times 1.25 = 15.0\text{ TB}$$
  $$\text{Capacity}_{\text{Year 2}} = 15.0\text{ TB} \times 1.25 = 18.75\text{ TB}$$
  $$\text{Capacity}_{\text{Year 3}} = 18.75\text{ TB} \times 1.25 = \mathbf{23.44\text{ TB}}$$

---

## 3. Data Protection and Redundancy Overhead
To guarantee 24/7 uninterrupted streaming and survive multiple concurrent disk hardware dropouts:
- **RAID 6 Dual-Parity Overhead**: In an 8-Data + 2-Parity configuration ($10$ drives), the parity overhead is:
  $$\text{Overhead}_{\text{RAID 6}} = \frac{10}{8} = 1.25\times$$
- **Filesystem Metadata & Inodes**: $5\%$ reserve.
- **Copy-on-Write Snapshot Reserve**: $15\%$ allocation for rolling snapshot rollbacks.
- **Total Usable Capacity Required (3-Year)**:
  $$\text{Usable}_{\text{3-Year}} = 23.44\text{ TB} \times (1 + 0.05 + 0.15) = 28.13\text{ TB}$$
- **Total Raw Drive Capacity Required**:
  $$\text{Raw}_{\text{Required}} = 28.13\text{ TB} \times 1.25 = \mathbf{35.16\text{ TB}}$$

### Physical Drive Selection
- Deploying $4\times 16\text{ TB}$ enterprise SAS HDDs in RAID 6 yields:
  $$\text{Raw Capacity} = 4 \times 16\text{ TB} = 64\text{ TB}$$
  $$\text{Usable Capacity} = (4 - 2) \times 16\text{ TB} = 32\text{ TB}$$
- **Result**: Provides 100% capacity headroom for the 3-year academic forecast.
