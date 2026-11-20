# Task 01: File-System Design Review

**Course:** CSA04 — Operating Systems  
**Assignment:** Assessment 4 (CO4) — Optimising File Storage and Disk Access  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Degree:** B.Tech Artificial Intelligence and Machine Learning  
**Institution:** SIMATS Engineering  
**Bloom's Level:** K5 – Evaluate  
**Official Marks:** 6 Marks  

---

## 1. Task Title
**Critical Architectural Review and Redesign of a Multi-User University File Storage System.**

---

## 2. Official Marks
**6 Marks** (Task 1.1: 2 Marks | Task 1.2: 3 Marks | Task 1.3: 1 Mark)

---

## 3. Objective
To critically analyze the fatal operational, security, and algorithmic flaws of a proposed flat single-directory contiguous file system for 12,000 students, and engineer a robust, scalable hierarchical directory architecture with POSIX access controls, extent-based dynamic allocation, and collaborative acyclic graph linking.

---

## 4. Problem Statement
A college proposes to store files for 12,000 students using:
- One flat directory for all files.
- Student-selected filenames without restriction.
- Contiguous allocation for every file.
- No access control because *"students will not look at each other's files."*

Evaluate the weaknesses, propose a comprehensive redesigned directory architecture, formulate a 100-word design defence, compare with cloud alternatives, and evaluate directory lookup algorithmic complexity.

---

## 5. Concepts Involved
- Flat vs. Hierarchical Directory Structures
- Contiguous vs. Dynamic Extent-Based Allocation
- POSIX Permission Modes (Owner, Group, Others)
- Access Control Lists (ACLs via `setfacl` / `getfacl`)
- Acyclic Graph Directories and Symbolic Linking (`ln -s`)
- Namespace Collision Mechanics
- Linear $O(N)$ Directory Scanning vs. $O(\log M)$ HTree Indexing
- Storage Quota Enforcement

---

## 6. Design and Algorithms
- **Directory Hierarchy:** Structured tree partitioned by Department $\to$ Academic Batch $\to$ Semester $\to$ Student ID $\to$ Course.
- **Access Control:** User home directories restricted to mode `700` (`rwx------`); shared project workspaces managed via POSIX ACLs with default inheritance bits.
- **Acyclic Graph Linking:** Team workspaces linked into individual home directories via symbolic links.

---

## 7. Source and Documentation Files
- `directory_tree.txt`: Full ASCII directory hierarchy with realistic sample paths.
- `file_system_evaluation.md`: Detailed technical critique of the 6 primary failure modes.
- `improved_directory_design.md`: Redesigned tree architecture, naming rules, and quota management.
- `access_control_design.md`: Permission bits, sticky bit dropbox, and POSIX ACL configuration.
- `cloud_storage_alternative.md`: Trade-off evaluation of Google Drive / OneDrive vs. on-premises storage.
- `complexity_analysis.md`: Formal mathematical derivation of $O(N)$ vs. $O(D \log M)$ lookup times.

---

## 8. Compilation Commands
*Documentation task — No standalone C compilation required for Task 1.*

---

## 9. Execution Commands
```bash
# View the redesigned directory tree
cat Task01/directory_tree.txt

# Inspect the access control design specifications
cat Task01/access_control_design.md
```

---

## 10. Expected Results
- Identification of 6 distinct architectural weaknesses.
- Redesigned directory hierarchy with depth $D=8$ eliminating collisions.
- Strict isolation preventing unauthorized access while facilitating group collaboration.
- Mathematical proof demonstrating a **$1,948\times$ search speedup**.

---

## 11. Calculations
- **Flat directory table size:** $600,000 \text{ files} \times 64 \text{ B} = 38.4 \text{ MB}$ (9,375 disk blocks to scan).
- **Hierarchical tree traversal:** Maximum 154 comparisons across 8 bounded levels.
- **Lookup speedup:** $\frac{300,000}{154} \approx 1,948\times$.

---

## 12. Observations
Flat single directories collapse catastrophically when multi-tenancy exceeds a few hundred files due to namespace pollution, linear disk seek penalties, and lack of security isolation.

---

## 13. Complexity Analysis
- **Flat Directory Search:** $O(N)$ average and worst-case time complexity.
- **Hierarchical Directory Lookup:** $O(D \cdot \log M)$ where $D$ is path depth and $M$ is entries per subdirectory.

---

## 14. Design Defence (Task 1.3 — Strict $\le$ 100 Words)
> **Our hierarchical directory design isolates each student's files into a private home directory secured with POSIX mode `700` (`rwx------`), making unauthorized access structurally impossible at the kernel VFS layer. Contiguous allocation is replaced with extent-based indexing to eliminate external fragmentation. Controlled collaboration is achieved without privacy compromise by establishing dedicated project directories governed by POSIX Access Control Lists (`setfacl`) and linked into students' personal trees via symbolic links. This architecture completely prevents filename collisions, accelerates directory lookups from $O(N)$ to $O(\log M)$, and guarantees data confidentiality alongside auditable, multi-user teamwork.**
*(98 words)*

---

## 15. Alternative Concept
Outsourcing to **Google Drive / Microsoft OneDrive** offers superior cross-campus availability and real-time collaboration, but introduces high network WAN build latency and recurring SaaS licensing costs compared to an on-premises local ZFS/NFS cluster.

---

## 16. Advantages
- Zero filename collisions.
- Near-instantaneous directory lookups.
- Cryptographic and POSIX access security.
- Granular per-student backup and restoration.

---

## 17. Limitations
- Requires administrative setup for automated LDAP directory provisioning upon student enrollment.

---

## 18. Conclusion
The proposed flat contiguous file system is technically indefensible for an institution of 12,000 students. The redesigned hierarchical model with extent allocation, POSIX permissions, and ACL-governed acyclic links provides the requisite scalability, performance, and security.

---

## 19. Evidence Image Placeholders
- `<!-- Add Gemini screenshot here: task01_directory_tree_inspection.png -->`
- `<!-- Add Gemini screenshot here: task01_acl_permissions_verification.png -->`

---

## 20. References
- Silberschatz, A., Galvin, P. B., & Gagne, G. *Operating System Concepts* (10th ed.), Chapter 13: File-System Interface.
- IEEE POSIX.1-2017 Standard: Directory Operations and Access Control Lists (`sys/stat.h`, `acl.h`).
