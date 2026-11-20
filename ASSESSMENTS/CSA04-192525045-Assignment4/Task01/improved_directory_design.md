# Task 1.2: Improved File-System Directory Architecture

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Hierarchical Multi-Level Directory Architecture and Collaboration Model  

---

## 1. Architectural Overview
To overcome the severe flaws of the flat contiguous model, we design an enterprise-grade **hierarchical, multi-level directory tree with acyclic graph sharing** built on modern POSIX standards.

```text
/college/
├── administration/
├── departments/
│   └── <dept_code>/
│       └── batch_<year>/
│           └── semester_<sem>/
│               ├── courses/
│               │   └── <course_code>/
│               │       ├── assignments/
│               │       └── course_materials/
│               └── students/
│                   └── <student_id>_<username>/
│                       ├── assignments/
│                       ├── projects/
│                       └── documents/
└── shared/
    ├── common_datasets/
    └── projects/
        └── <project_id>/
```

---

## 2. Key Structural Components

### 2.1 Hierarchical Decomposition
- **Namespace Partitioning:** By organizing files across Department $\to$ Academic Year $\to$ Semester $\to$ Student ID, each student directory holds an isolated namespace.
- **Directory Fan-out:** No single directory contains more than 100–500 child entries, enabling high-speed $B$-tree or HTree indexing in modern file systems (e.g. ext4, XFS) with $O(\log M)$ lookup complexity.

### 2.2 Student Directory Layout & Naming Rules
Every student receives a private home directory formatted as `/college/departments/<dept>/batch_<year>/semester_<sem>/students/<student_id>_<username>/`.
- **Naming Rule:** `<course_code>_<assessment_type>_<submission_name>_v<version>.<ext>`  
  *Example:* `csa04_os_assignment4_v1.0.tar.gz`
- **Subdirectories:**
  - `assignments/`: Organized per course code (`csa04_os/`, `csa05_dbms/`).
  - `projects/`: Holds private components and links to group workspaces.
  - `documents/`: Personal academic credentials, resumes, transcripts.

### 2.3 Storage Allocation Strategy
- Replaces contiguous allocation with **extent-based dynamic indexed allocation** (as used in ext4, XFS, and Btrfs).
- Allows files to grow seamlessly from a few kilobytes to gigabytes without external fragmentation or pre-allocation penalties.
- Enforces user-level and group-level disk quotas (`edquota`) to prevent individual users from monopolizing physical storage.

### 2.4 Collaboration via Acyclic Graph Linking
- Group project workspaces reside in `/college/shared/projects/<project_id>/`.
- Students access project assets directly via **POSIX Symbolic Links (`ln -s`)** inside their local `projects/` directory:
  ```bash
  ln -s /college/shared/projects/2026_aiml_p04 /college/departments/aiml/batch_2023/semester_4/students/192525045_sachin/projects/team_hydra_project
  ```
- This forms an **Acyclic Graph Directory Structure** that facilitates collaborative read/write access without duplicating storage blocks or causing reference cycles.

### 2.5 Backup and Recovery Integration
- **Hourly/Daily Snapshots:** Enabled via underlying volume managers (LVM / ZFS snapshots) without freezing running services.
- **Per-Student Granular Restoration:** Backup tools can target `/college/.../students/<student_id>/` directly, restoring an individual student's corrupted file in seconds without touching global storage.
