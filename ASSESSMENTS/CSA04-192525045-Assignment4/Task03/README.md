# Task 03: Modern File Systems

**Course:** CSA04 — Operating Systems  
**Assignment:** Assessment 4 (CO4) — Optimising File Storage and Disk Access  
**Student Name:** S. Sachin  
**Register Number:** 192525045  
**Degree:** B.Tech Artificial Intelligence and Machine Learning  
**Institution:** SIMATS Engineering  
**Bloom's Level:** K5 – Evaluate  
**Official Marks:** 9 Marks  

---

## 1. Task Title
**Comparative Evaluation of Modern File Systems, Journaling Consistency, and Device-Specific Architecture.**

---

## 2. Official Marks
**9 Marks** (Task 3.1: 3 Marks | Task 3.2: 2 Marks | Task 3.3: 3 Marks | Task 3.4: 1 Mark)

---

## 3. Objective
To systematically compare modern file systems (FAT32, NTFS, ext4, XFS, Btrfs, ZFS, APFS), analyze power-failure consistency mechanisms (journaling vs. copy-on-write), evaluate ext4 journaling modes, provide justified file-system choices for four distinct hardware environments, and build safe inspection tools.

---

## 4. Problem Statement
Select and justify file systems for:
1. Cross-platform USB pen drive (Windows and Linux).
2. High-performance Linux database server.
3. Everyday laptop workstation.
4. Enterprise backup server requiring snapshots.

Explain crash recovery, journaling modes (`journal`, `ordered`, `writeback`), and inspect real system metadata using standard Linux utilities.

---

## 5. Concepts Involved
- Modern File Systems Architecture (ext4, XFS, ZFS, Btrfs, NTFS, FAT32, APFS)
- Maximum File and Volume Capacity Scaling
- Crash Consistency and Power-Failure Hazards
- Write-Ahead Logging (Journaling) vs. Copy-on-Write (CoW)
- ext4 Journaling Modes: `data=journal`, `data=ordered`, `data=writeback`
- Inodes, Allocation Groups, Bit rot, and End-to-End Data Checksums
- Linux Superblock & Inode Inspection Utilities (`stat`, `df -i`, `tune2fs`)

---

## 6. Design and Algorithms
- **Comparison Matrix:** 7 modern file systems evaluated across 9 technical parameters.
- **Consistency Engine:** Models write-ahead logging transactions vs. atomic tree commits.
- **Inspection Script:** Safe read-only Bash script querying inode tables and block sizing.

---

## 7. Source and Documentation Files
- `filesystem_comparison.md`: In-depth comparison matrix covering all 7 modern file systems.
- `journaling_analysis.md`: Detailed failure mechanics, WAL journaling, CoW, and ext4 modes.
- `filesystem_recommendations.md`: Justified choices for USB, Database, Laptop, and Backup.
- `filesystem_inspection.sh`: Executable read-only script querying `stat`, `df -i`, `ls -li`.
- `filesystem_commands.md`: Reference guide for system administration inspection commands.
- `complexity_analysis.md`: Algorithmic analysis comparing CoW wandering trees vs. in-place journaling.
- `expected_output.txt`: Verified sample output for filesystem inspection.

---

## 8. Compilation Commands
*Shell script and documentation task — No C compilation required.*

---

## 9. Execution Commands
```bash
# Make script executable and run safe inspection
chmod +x Task03/filesystem_inspection.sh
./Task03/filesystem_inspection.sh .
```

---

## 10. Expected Results
- Comprehensive comparison matrix detailing capacities and strengths.
- Rigorous explanation of crash consistency.
- Justified selections: exFAT (USB), XFS (Database), ext4 (Laptop), ZFS (Backup Server).
- Verified read-only execution output showing block size (4096 B) and inode distribution.

---

## 11. Calculations
- Block sizing verification: Standard 4 KB blocks with 256-byte inode structures.

---

## 12. Observations
- `data=ordered` represents the ideal balance between performance and consistency.
- Copy-on-Write systems (ZFS/Btrfs) excel at snapshots and archival, but incur write amplification penalties under random database write workloads.

---

## 13. Complexity Analysis
- In-Place Journaled Writes (ext4/XFS): $O(1)$ block write.
- Copy-on-Write Tree Propagation (ZFS/Btrfs): $O(\log_B N)$ tree pointer rewrite.

---

## 14. Alternative Concept: ZFS/Btrfs vs. ext4 for Databases
Using CoW for databases eliminates `fsck` recovery delays and prevents bit rot, but causes table fragmentation. Mitigated by disabling CoW on database directories (`chattr +C` on Btrfs).

---

## 15. Advantages
- Accurate, evidence-based recommendations tailored to real hardware constraints.
- Non-destructive execution guaranteeing host safety.

---

## 16. Limitations
- Superblock inspection commands (`tune2fs`) require root privileges on Linux.

---

## 17. Conclusion
Storage selection must align with workload patterns. **XFS** dominates high-concurrency database I/O, **exFAT** ensures portable USB access, **ext4** powers robust laptops, and **ZFS** provides unmatched snapshot and integrity capabilities for backup infrastructure.

---

## 18. Evidence Image Placeholders
- `<!-- Add Gemini screenshot here: task03_filesystem_comparison_table.png -->`
- `<!-- Add Gemini screenshot here: task03_filesystem_inspection_terminal.png -->`

---

## 19. References
- Bovet, D. P., & Cesati, M. *Understanding the Linux Kernel* (3rd ed.), Chapter 18: The Ext2 and Ext3 Filesystems.
- Linux Kernel Documentation: `Documentation/filesystems/ext4/journaling.rst`.
- Bonwick, J., & Moore, B. *ZFS: The Last Word in File Systems*. Sun Microsystems Technical Report.
