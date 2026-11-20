# Computational Complexity Analysis of Disk Scheduling Algorithms

## 1. Complexity Comparison Table

| Algorithm | Time Complexity (Naive) | Time Complexity (Optimized) | Space Complexity | Decision Determinism |
| :--- | :---: | :---: | :---: | :---: |
| **FCFS** | $O(N)$ | $O(N)$ | $O(1)$ | Deterministic |
| **SSTF** | $O(N^2)$ | $O(N \log N)$ (with Balanced BST) | $O(N)$ | Deterministic |
| **SCAN** | $O(N \log N)$ (Sorting queue) | $O(N \log N)$ | $O(N)$ | Deterministic |
| **C-SCAN** | $O(N \log N)$ | $O(N \log N)$ | $O(N)$ | Deterministic |
| **LOOK** | $O(N \log N)$ | $O(N \log N)$ | $O(N)$ | Deterministic |
| **C-LOOK** | $O(N \log N)$ | $O(N \log N)$ | $O(N)$ | Deterministic |

---

## 2. In-Depth Algorithmic Analysis

### 2.1 SSTF
- **Naive Implementation**: An outer loop of $N$ iterations searching an array of size $N$ for the minimum absolute difference:
  $$T(N) = \sum_{k=1}^N (N - k) = \frac{N(N-1)}{2} = O(N^2)$$
- **Optimized Implementation**: Inserting requests into a self-balancing Red-Black Tree ($O(N \log N)$). Finding the in-order predecessor or successor of the current head position takes $O(\log N)$ time, yielding overall $O(N \log N)$ dispatch.

### 2.2 Elevator Algorithms (SCAN, C-SCAN, LOOK, C-LOOK)
- Sorting the request array of size $N$ requires $O(N \log N)$ using Quicksort or Mergesort.
- Traversing the sorted list in directional passes requires a single linear scan: $O(N)$.
- Total runtime is dominated by sorting: $O(N \log N)$.
- Space complexity is $O(N)$ to buffer pending requests.
