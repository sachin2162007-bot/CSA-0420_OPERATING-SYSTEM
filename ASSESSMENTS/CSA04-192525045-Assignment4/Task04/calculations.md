# Task 04: Disk Scheduling Step-by-Step Mathematical Calculations

## 1. Official Parameters
- **Disk Size**: 200 Cylinders ($0$ to $199$).
- **Starting Head Position**: $53$.
- **Pending Request Sequence**: $[98, 183, 37, 122, 14, 124, 65, 67]$.

---

## 2. Full Calculation Traces

### 2.1 First-Come, First-Served (FCFS)
| Step | From Cylinder | To Cylinder | Head Movement (Cylinders) | Running Total |
| :---: | :---: | :---: | :---: | :---: |
| 1 | 53 | 98 | $|98 - 53| = 45$ | 45 |
| 2 | 98 | 183 | $|183 - 98| = 85$ | 130 |
| 3 | 183 | 37 | $|37 - 183| = 146$ | 276 |
| 4 | 37 | 122 | $|122 - 37| = 85$ | 361 |
| 5 | 122 | 14 | $|14 - 122| = 108$ | 469 |
| 6 | 14 | 124 | $|124 - 14| = 110$ | 579 |
| 7 | 124 | 65 | $|65 - 124| = 59$ | 638 |
| 8 | 65 | 67 | $|67 - 65| = 2$ | **640** |

$$\text{Average Seek} = \frac{640}{8} = 80.0\text{ cylinders/request}$$

---

### 2.2 Shortest Seek Time First (SSTF)
| Step | Current Head | Unserviced Pool | Closest Request | Distance | Running Total |
| :---: | :---: | :--- | :---: | :---: | :---: |
| 1 | 53 | $\{98, 183, 37, 122, 14, 124, 65, 67\}$ | 65 | $|65-53|=12$ | 12 |
| 2 | 65 | $\{98, 183, 37, 122, 14, 124, 67\}$ | 67 | $|67-65|=2$ | 14 |
| 3 | 67 | $\{98, 183, 37, 122, 14, 124\}$ | 37 | $|37-67|=30$ | 44 |
| 4 | 37 | $\{98, 183, 122, 14, 124\}$ | 14 | $|14-37|=23$ | 67 |
| 5 | 14 | $\{98, 183, 122, 124\}$ | 98 | $|98-14|=84$ | 151 |
| 6 | 98 | $\{183, 122, 124\}$ | 122 | $|122-98|=24$ | 175 |
| 7 | 122 | $\{183, 124\}$ | 124 | $|124-122|=2$ | 177 |
| 8 | 124 | $\{183\}$ | 183 | $|183-124|=59$ | **236** |

$$\text{Average Seek} = \frac{236}{8} = 29.5\text{ cylinders/request}$$

---

### 2.3 SCAN (Elevator)
Head moves downward from $53 \to 0$, services requests encountered ($37, 14$), touches boundary $0$, reverses upward to $183$, servicing $65, 67, 98, 122, 124, 183$.
$$\text{Downward movement}: 53 - 0 = 53\text{ cylinders}$$
$$\text{Upward movement}: 183 - 0 = 183\text{ cylinders}$$
$$\text{Total Movement} = 53 + 183 = \mathbf{236\text{ cylinders}}$$

---

### 2.4 C-SCAN (Circular SCAN)
Head moves upward from $53 \to 199$, servicing $65, 67, 98, 122, 124, 183$.
Touches upper boundary $199$.
Fast return jump to lower boundary $0$ ($199$ cylinders).
Services upward from $0 \to 37$, servicing $14, 37$.
$$\text{Upward pass}: 199 - 53 = 146\text{ cylinders}$$
$$\text{Return jump}: 199 - 0 = 199\text{ cylinders}$$
$$\text{Second upward pass}: 37 - 0 = 37\text{ cylinders}$$
$$\text{Total Movement} = 146 + 199 + 37 = \mathbf{382\text{ cylinders}}$$
