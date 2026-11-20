# Comprehensive Master Calculations Repository: Tasks 01 to 06

## 1. Task 02: Inode Capacity and Traversal Calculations
- **Block Size**: 4 KB ($4,096\text{ bytes}$). Pointer Size: 4 bytes.
- Pointers per indirect block: $4,096 / 4 = 1,024\text{ pointers}$.
- **Tiers of Inode**:
  1. Direct Blocks ($12$ pointers):
     $$12 \times 4\text{ KB} = \mathbf{48\text{ KB}}$$
  2. Single Indirect Block ($1$ pointer):
     $$1,024 \times 4\text{ KB} = \mathbf{4\text{ MB}}$$
  3. Double Indirect Block ($1$ pointer):
     $$1,024 \times 1,024 \times 4\text{ KB} = 1,048,576 \times 4\text{ KB} = \mathbf{4\text{ GB}}$$
  4. Triple Indirect Block ($1$ pointer):
     $$1,024 \times 1,024 \times 1,024 \times 4\text{ KB} = 1,073,741,824 \times 4\text{ KB} = \mathbf{4\text{ TB}}$$
- **Maximum File Size Supported**:
  $$\text{Max Size} = 48\text{ KB} + 4\text{ MB} + 4\text{ GB} + 4\text{ TB} = \mathbf{4.004004\text{ TiB} / 4.4025\text{ TB}}$$

### Inode Traversal to Byte Offset 4,000,000,000 (4 GB File)
- Target Block Index:
  $$\text{Block Index} = \left\lfloor \frac{4,000,000,000}{4,096} \right\rfloor = 976,562$$
- Checking Range:
  - Direct blocks: $0 \dots 11$ ($12$ blocks).
  - Single indirect blocks: $12 \dots 1,035$ ($1,024$ blocks).
  - Double indirect blocks: $1,036 \dots 1,049,611$ ($1,048,576$ blocks).
- Since $1,036 \le 976,562 \le 1,049,611$, byte offset $4,000,000,000$ resides in the **Double Indirect Block**.
- Offset within double indirect space:
  $$\text{Relative Block} = 976,562 - 1,036 = 975,526$$
  $$\text{First-Level Index} = \left\lfloor \frac{975,526}{1,024} \right\rfloor = \mathbf{952}$$
  $$\text{Second-Level Index} = 975,526 \pmod{1,024} = \mathbf{678}$$

---

## 2. Task 02: Free-Space Bit Vector Sizing
- **Disk Size**: 4 TB ($4 \times 10^{12}\text{ bytes}$). Block Size: 4 KB ($4,096\text{ bytes}$).
- Total Disk Blocks:
  $$\text{Total Blocks} = \frac{4 \times 10^{12}\text{ bytes}}{4,096\text{ bytes}} = 976,562,500\text{ blocks}$$
- Memory Required for Bit Vector (1 bit per block):
  $$\text{Size}_{\text{bytes}} = \frac{976,562,500\text{ bits}}{8\text{ bits/byte}} = 122,070,312.5\text{ bytes}$$
  $$\text{Size}_{\text{MB}} = \frac{122,070,312.5}{1,000,000} \approx \mathbf{122.07\text{ MB}}$$
  $$\text{Size}_{\text{MiB}} = \frac{122,070,312.5}{1,048,576} \approx \mathbf{116.42\text{ MiB}}$$

---

## 3. Task 04: Disk Scheduling Step-by-Step Movement Calculations
- Initial Head: 53. Requests: `98, 183, 37, 122, 14, 124, 65, 67`.
- **FCFS**:
  $$|53-98| + |98-183| + |183-37| + |37-122| + |122-14| + |14-124| + |124-65| + |65-67|$$
  $$= 45 + 85 + 146 + 85 + 108 + 110 + 59 + 2 = \mathbf{640\text{ cylinders}}$$
- **SSTF**:
  $$|53-65| + |65-67| + |67-37| + |37-14| + |14-98| + |98-122| + |122-124| + |124-183|$$
  $$= 12 + 2 + 30 + 23 + 84 + 24 + 2 + 59 = \mathbf{236\text{ cylinders}}$$
- **SCAN** (Downward to 0, then Upward to 183):
  $$(53 - 0) + (183 - 0) = 53 + 183 = \mathbf{236\text{ cylinders}}$$
- **C-SCAN** (Upward to 199, Return to 0, Upward to 37):
  $$(199 - 53) + (199 - 0) + (37 - 0) = 146 + 199 + 37 = \mathbf{382\text{ cylinders}}$$

---

## 4. Task 05: RAID 5 Rebuild URE Probability
- Array: $8\times 16\text{ TB}$ drives. Single failure occurs.
- Surviving drives to read: $7\times 16\text{ TB} = 112\text{ TB}$.
- Total bits read: $112 \times 10^{12} \times 8 = 8.96 \times 10^{14}\text{ bits}$.
- URE Rate: $10^{-14}$.
- Probability of Zero UREs:
  $$P(\text{Success}) = (1 - 10^{-14})^{8.96 \times 10^{14}} \approx e^{-8.96} = 0.0001284$$
- **Probability of Rebuild Failure**:
  $$P(\text{Catastrophic Data Loss}) = 1 - 0.0001284 = \mathbf{99.987\%}$$

---

## 5. Task 06: Video Streaming Capacity, Throughput, and Cloud Egress
- **Peak Throughput**: $25,000 \times 4.0\text{ Mbps} = 100,000\text{ Mbps} = \mathbf{100\text{ Gbps}} = \mathbf{12.5\text{ GB/s}}$.
- **Monthly Cloud Egress**:
  $$12.5\text{ GB/s} \times 3,600\text{ s/hr} \times 8\text{ hrs/day} \times 25\text{ days/mo} = 9,000,000\text{ GB} = \mathbf{9.0\text{ PB/month}}$$
- **Monthly Cloud Egress Bill** (@ $\$0.06/\text{GB}$):
  $$9,000,000 \times \$0.06 = \mathbf{\$540,000\text{ / month}}$$
