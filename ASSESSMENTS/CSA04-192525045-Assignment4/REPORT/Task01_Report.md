# Task 01: File-System Design Review Report

**Course**: CSA04 – Operating Systems (CO4)  
**Student**: S. Sachin (192525045)  
**Marks Allocated**: 6 Marks  

## 1. Evaluation of Legacy Flat Contiguous File System
The legacy setup stores 12,000 student project folders in a single directory using contiguous allocation on a 2 TB hard disk.
- **Directory Lookup Degradation**: Linear scan $O(N)$ across 12,000 directory entries causes heavy lock contention in VFS.
- **External Fragmentation**: Continuous creation and deletion of unevenly sized project files creates fragmented gaps too small to reuse.
- **File Growth Blocking**: Contiguous allocation requires pre-allocating fixed boundaries; files cannot grow dynamically if adjacent blocks are occupied.
- **Namespace Clashes**: All students share one namespace, causing filename collisions for identical file names (`main.c`).
- **Access Control Vulnerability**: Coarse POSIX permissions fail to separate student, instructor, and automated grading access.

## 2. Improved Hierarchical Directory Architecture
Structured hierarchy organizing by academic year, department, register number, and project subdirectories reduces directory lookup time to $O(\log N)$ using indexed B-trees.

## 3. POSIX Access Control Lists (ACLs)
Granular permissions configure student read/write isolation, evaluator read/grade access, autograder sandbox access, and default permission inheritance across newly created directories.

## 4. Official 98-Word Architectural Defense
> The proposed hierarchical directory structure with indexed allocation and POSIX access control lists completely resolves the critical bottlenecks of the legacy flat contiguous file system. Dividing twelve thousand students into structured department and batch subdirectories reduces directory lookup time from linear O(N) search to logarithmic O(log N) indexed B-tree search. Ext4 extent-based indexed allocation completely eliminates external disk fragmentation and supports dynamic, unrestricted file growth. Granular access control lists enforce strict least-privilege security between students and faculty evaluators, while preserving high-speed local POSIX file-locking operations that cloud storage alternatives cannot provide.
