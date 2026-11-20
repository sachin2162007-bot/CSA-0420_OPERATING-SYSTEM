# Disk Scheduler Comparative Analysis

## 1. Official Problem Parameters
- **Disk Geometry**: 200 Cylinders (numbered $0$ to $199$).
- **Initial Head Position**: Cylinder $53$.
- **Pending Request Queue**: `98, 183, 37, 122, 14, 124, 65, 67` ($N = 8$).

---

## 2. Comparative Evaluation Matrix

| Algorithm | Total Head Movement (cylinders) | Service Order Trace | Strengths | Weaknesses & Vulnerabilities |
| :--- | :---: | :--- | :--- | :--- |
| **FCFS** | **640** | $53 \to 98 \to 183 \to 37 \to 122 \to 14 \to 124 \to 65 \to 67$ | Absolutely fair; zero starvation; trivial $O(1)$ dispatch. | Wild oscillations across platters; catastrophic seek overhead ($640$ cyls). |
| **SSTF** | **236** | $53 \to 65 \to 67 \to 37 \to 14 \to 98 \to 122 \to 124 \to 183$ | Minimizes immediate seek time; low average response time under light load. | Prone to **starvation** of peripheral requests when central clusters arrive. |
| **SCAN** | **236** | $53 \to 37 \to 14 \to 0 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183$ | Upper bound on wait time; eliminates starvation; smooth directional sweeps. | Biased toward center tracks; wastes time sweeping to cylinder $0$ when no requests exist there. |
| **C-SCAN** | **382** | $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 199 \to 0 \to 14 \to 37$ | Uniform wait time across all cylinders; treats tracks as circular ring. | Higher total seek due to return sweep ($199$ cyls jump from edge to edge). |
| **LOOK** | **208** | $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 37 \to 14$ | Reverses at highest/lowest actual request; avoids traveling to physical platter limits ($0$ and $199$). | Non-uniform waiting distribution between inward and outward passes. |
| **C-LOOK** | **322** | $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 14 \to 37$ | Circular fairness; avoids traveling to platter edges ($0$ and $199$). | Slight overhead on circular return jump ($183 \to 14 = 169$ cyls). |

---

## 3. Detailed Trace & Calculation Breakdown

### 3.1 FCFS (First-Come First-Served)
$$\Delta = |53-98| + |98-183| + |183-37| + |37-122| + |122-14| + |14-124| + |124-65| + |65-67|$$
$$\Delta = 45 + 85 + 146 + 85 + 108 + 110 + 59 + 2 = \mathbf{640\text{ cylinders}}$$

### 3.2 SSTF (Shortest Seek Time First)
1. At $53$: candidates are $65$ ($12$) vs $37$ ($16$). Pick $65$.
2. At $65$: candidate $67$ ($2$). Pick $67$.
3. At $67$: candidate $37$ ($30$) vs $98$ ($31$). Pick $37$.
4. At $37$: candidate $14$ ($23$) vs $98$ ($61$). Pick $14$.
5. At $14$: candidate $98$ ($84$). Pick $98$.
6. At $98$: candidate $122$ ($24$). Pick $122$.
7. At $122$: candidate $124$ ($2$). Pick $124$.
8. At $124$: candidate $183$ ($59$). Pick $183$.
$$\Delta = 12 + 2 + 30 + 23 + 84 + 24 + 2 + 59 = \mathbf{236\text{ cylinders}}$$

### 3.3 SCAN (Downward Sweep to 0, then Upward to 183)
$$\Delta = (53 - 0) + (183 - 0) = 53 + 183 = \mathbf{236\text{ cylinders}}$$

### 3.4 C-SCAN (Upward to 199, Jump to 0, Upward to 37)
$$\Delta = (199 - 53) + (199 - 0) + (37 - 0) = 146 + 199 + 37 = \mathbf{382\text{ cylinders}}$$
