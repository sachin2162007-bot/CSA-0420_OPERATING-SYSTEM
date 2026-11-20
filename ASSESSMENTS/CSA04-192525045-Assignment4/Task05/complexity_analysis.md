# Algorithmic Complexity in Distributed and Cloud Storage Systems

## 1. Algorithmic Complexity Table

| Operation | Centralized NFS | Master-Worker DFS (HDFS) | Decentralized DFS (Ceph) | Cloud Object Store (AWS S3) |
| :--- | :---: | :---: | :---: | :---: |
| **Object / File Lookup** | $O(d)$ (RPC traversal per directory depth $d$) | $O(1)$ (In-memory Hash Table at NameNode) | $\mathbf{O(\log P)}$ (CRUSH mathematical calculation, zero network lookup!) | $O(1)$ (Consistent hashing across distributed key ring) |
| **Write Replication Cost** | $O(1)$ (Local disk write only) | $O(K \cdot B)$ (Pipelined stream of $B$ bytes to $K$ replicas) | $O(K \cdot B)$ (Parallel primary-to-replica network dispatch) | $O(K \cdot B)$ (Synchronous multi-AZ commit) |
| **Cluster Metadata Scaling** | $O(F)$ (File count bound to single disk inode limit) | $O(B)$ (Heap memory limit: $\approx 150\text{ bytes} \times \text{blocks}$) | $\mathbf{O(1)}$ (Independent of total object count; bound only to cluster topology map) | $O(\infty)$ (Managed cloud metadata partition tables) |
| **Node Failure Recovery** | $O(\text{Disk Size})$ (Serial copy to spare) | $O\left(\frac{V}{M \cdot \text{BW}}\right)$ (Distributed parallel reconstruct across DataNodes) | $O\left(\frac{V}{N \cdot \text{BW}}\right)$ (Massively parallel declustered rebuild across all OSDs) | Automated internal background repair |

---

## 2. In-Depth Mathematical Complexity Analysis

### 2.1 The Ceph CRUSH Placement Function
Ceph eliminates centralized directory lookups using the deterministic CRUSH placement function:
$$\text{OSD\_List} = \text{CRUSH}(\text{ClusterMap}, \text{RuleSet}, \text{PG\_ID}(\text{Hash}(\text{ObjectID})))$$
- Where $\text{PG\_ID} = \text{JenkinsHash}(\text{ObjectID}) \pmod{\text{Total\_PGs}}$.
- Selecting replica targets traversing an $M$-level hierarchy of weight-balanced trees requires:
  $$T_{\text{CRUSH}} = O(d \cdot \log W)$$
  where $d$ is hierarchy depth ($\le 5$) and $W$ is average branch factor ($\le 64$).
- **Computational Time**: Less than $2\text{ }\mu\text{s}$ of CPU time on the client machine!
- **Network Metadata Traffic**: **Zero packets exchanged**. Clients contact target OSDs directly.

### 2.2 Rebuild Time Complexity: Conventional RAID vs. Ceph Declustered Parity
- In traditional RAID 5/6, an entire spare drive is rebuilt from $N-1$ drives through a single storage controller:
  $$T_{\text{RAID\_Rebuild}} = \frac{\text{Capacity}_{\text{drive}}}{\min(\text{Drive\_Write\_BW}, \text{Bus\_BW})} \approx \frac{16\text{ TB}}{150\text{ MB/s}} \approx \mathbf{30\text{ hours}}$$
- In Ceph, data from a failed OSD is distributed across hundreds of Placement Groups shared among $P$ distinct OSDs:
  $$T_{\text{Ceph\_Rebuild}} = \frac{\text{Capacity}_{\text{OSD}}}{P \cdot \text{Network\_BW}} \approx \frac{16\text{ TB}}{100 \times 1\text{ Gbps}} \approx \mathbf{21\text{ minutes}}$$
- **Result**: Ceph reduces the critical window of vulnerability by over $98\%$.
