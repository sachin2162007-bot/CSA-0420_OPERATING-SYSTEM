# Task 1.1: Evaluation of the Proposed College File Organisation

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Critical Analysis of the Flat Single-Directory Contiguous Model  

---

## 1. Executive Summary & Problem Context
A university infrastructure team proposed a centralized student file-storage system serving **12,000 enrolled students** with the following technical parameters:
1. **One single flat directory** holding every file from every student.
2. **Student-selected filenames** with zero namespace restrictions (e.g. `assignment1.c`, `project.docx`).
3. **Contiguous disk allocation** for all files.
4. **No access control mechanisms** based on the naïve social assumption that *"students will not look at each other's files."*

This proposal exhibits fundamental structural, architectural, and security flaws that violate basic operating systems principles. A rigorous technical breakdown of the six primary failure modes is detailed below.

---

## 2. In-Depth Failure Mode Analysis

### 2.1 Catastrophic Filename Collisions (Namespace Collapse)
- **Technical Issue:** In a flat directory, the filename serves as the unique primary key in the directory table. When multiple users share a flat namespace without prefixing or partitioning, collisions are mathematically inevitable.
- **Consequence:** If Student A creates `assignment1.c` and Student B subsequently saves a file named `assignment1.c`, the operating system will either:
  1. Overwrite Student A's file silently, resulting in irrevocable data loss.
  2. Fail the second write with `EEXIST` (File exists), preventing 11,999 other students from submitting work under common assignment naming conventions.
- **System Impact:** Total breakdown of academic coursework submission integrity.

### 2.2 Extreme Directory Search Latency ($O(N)$ Exhaustion)
- **Technical Issue:** Assuming each student stores an average of 50 academic files, the single flat directory must track:
  $$\text{Total Directory Entries} = 12,000 \times 50 = 600,000\text{ files}$$
- **Consequence:** 
  - Standard directory search on an un-indexed linear directory table requires scanning directory entries sequentially ($O(N)$ time complexity).
  - Opening, reading, or creating any file requires traversing hundreds of thousands of directory blocks from disk into RAM.
  - Disk head thrashing on rotational HDDs or excessive metadata memory locking on SSDs severely degrades I/O operations per second (IOPS), rendering interactive terminal shell commands (`ls`, tab-completion) completely unresponsive (taking several minutes per invocation).

### 2.3 File Growth Stagnation & Severe External Fragmentation under Contiguous Allocation
- **Technical Issue:** Contiguous allocation mandates that all blocks of a file occupy contiguous physical disk blocks ($b, b+1, b+2, \dots, b+k-1$).
- **Consequence:**
  - Files cannot expand dynamically. If Student A allocates 4 KB for a source code file and subsequently adds comments or code expanding it to 8 KB, the file cannot grow if block $b+1$ is allocated to another student.
  - The OS must either reject the write (`ENOSPC`) or perform an expensive relocation: search for a new free contiguous run of blocks, copy the entire file contents, update the directory entry, and release the old blocks.
  - Over time, repeated file deletions and creations fragment the storage into thousands of small, non-contiguous free block "holes" (external fragmentation), causing allocation requests for moderate-sized project archives to fail despite vast total aggregate free disk space.

### 2.4 Complete Privacy, Confidentiality, and Integrity Violation
- **Technical Issue:** Omitting access control lists (ACLs) and permission bits removes all isolation between distinct security domains.
- **Consequence:**
  - Any student can view, copy, alter, or delete any other student's exam submissions, private correspondence, or intellectual property.
  - Plagiarism becomes trivial and untraceable. Malicious actors or accidental script errors (e.g. `rm -rf *`) can wipe the entire 600,000-file repository in a single command.
  - The system violates fundamental statutory data protection mandates (e.g., GDPR, FERPA) regarding educational records privacy.

### 2.5 Infeasible Backup, Archival, and Disaster Recovery
- **Technical Issue:** System-level backups operate efficiently through hierarchical snapshots, incremental change tracking, and per-user directory trees.
- **Consequence:**
  - In a flat 600,000-file directory, performing differential or incremental backups requires scanning the entire directory table on every pass.
  - Selective recovery is impossible: recovering a single student's accidental deletion requires restoring from a monolithic multi-terabyte tape or image, halting file system access for the entire university.
  - A single corrupted directory metadata block corrupts the entire college's file index.

### 2.6 Impossibility of Collaborative Group Projects
- **Technical Issue:** Academic curricula require team collaboration where specific subsets of students share read/write access to project repositories.
- **Consequence:**
  - Without group ownership bits (GID), access control lists (ACLs), or symbolic/hard directory links, selective group sharing cannot exist.
  - The only options are zero sharing or global public read/write access, leaving collaborative repositories vulnerable to sabotage.

---

## 3. Summary Assessment Matrix
| Evaluation Dimension | Proposed Flat System | Impact Severity | Academic / Operational Risk |
| :--- | :--- | :--- | :--- |
| **Namespace Hygiene** | Flat single namespace | Critical | 100% collision rate for standard filenames |
| **Lookup Performance** | Linear $O(N)$ traversal | High | Server unresponsiveness, shell timeouts |
| **Storage Allocation** | Contiguous allocation | Critical | Dynamic expansion blocked, external fragmentation |
| **Access Security** | Zero permissions / ACLs | Fatal | Data loss, plagiarism, unauthorized modification |
| **Disaster Recovery** | Monolithic flat table | High | Inability to restore individual student files |
| **Collaboration** | No group constructs | Medium | Group work and peer review impossible |
