# Task 1.2 (Continued): Access Control and Permission Engineering

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Dual-Layer Access Control Lists (ACLs) and POSIX Security Model  

---

## 1. Security Architecture
To guarantee strict academic privacy while facilitating managed collaboration, the improved system implements a **Dual-Layer Security Architecture**:
1. **Layer 1: Standard POSIX Permission Bits (Owner / Group / Others)**
2. **Layer 2: Granular POSIX Access Control Lists (ACLs via `setfacl` / `getfacl`)**

---

## 2. Permission Bit Strategy

### 2.1 Student Home Directory Permissions
Every student directory is set to mode `700` (`rwx------`):
```bash
chmod 700 /college/departments/aiml/batch_2023/semester_4/students/192525045_sachin
```
- **Owner (Student):** Full read, write, and execute permissions (`rwx`).
- **Group:** No permissions (`---`).
- **Others (All students):** Zero access (`---`).
- **Result:** No other student can list, enter, or inspect contents of another student's home tree.

### 2.2 Course Materials and Submission Dropboxes
- **Course Material (`/college/.../courses/csa04_os/lecture_slides/`):**  
  Mode `755` (`rwxr-xr-x`): Instructor has full control; all enrolled students have read and execute permissions.
- **Assignment Submission Box (`/college/.../courses/csa04_os/dropbox/`):**  
  Mode `1733` (`rwx-wx-wt`): Contains the **Sticky Bit (`+t`)** and write/execute permissions for the student group. Students can deposit submission archives, but cannot delete or overwrite peers' submissions.

---

## 3. POSIX Access Control Lists (ACLs) for Collaborative Workspaces

Collaborative project workspaces require multi-user read/write access while barring non-team students and preserving faculty audit capability.

### Configuration for Team Project `2026_aiml_p04`:
```bash
# 1. Create project workspace
mkdir -p /college/shared/projects/2026_aiml_p04

# 2. Grant full ownership to Project Lead (Sachin)
chown 192525045:aiml_students /college/shared/projects/2026_aiml_p04

# 3. Restrict base permissions (No access for general public)
chmod 770 /college/shared/projects/2026_aiml_p04

# 4. Apply POSIX ACLs for specific team members (Read/Write/Execute)
setfacl -m u:192525045:rwx /college/shared/projects/2026_aiml_p04
setfacl -m u:192525046:rwx /college/shared/projects/2026_aiml_p04
setfacl -m u:192525047:rwx /college/shared/projects/2026_aiml_p04

# 5. Grant Faculty Supervisor Read-Only auditing rights
setfacl -m u:faculty_advisor:r-x /college/shared/projects/2026_aiml_p04

# 6. Apply Default ACLs to ensure newly created files inherit team permissions automatically
setfacl -d -m u:192525045:rwx /college/shared/projects/2026_aiml_p04
setfacl -d -m u:192525046:rwx /college/shared/projects/2026_aiml_p04
setfacl -d -m u:192525047:rwx /college/shared/projects/2026_aiml_p04
setfacl -d -m u:faculty_advisor:r-x /college/shared/projects/2026_aiml_p04
setfacl -d -m o::--- /college/shared/projects/2026_aiml_p04
```

---

## 4. Verification via `getfacl`
```text
# file: /college/shared/projects/2026_aiml_p04
# owner: 192525045
# group: aiml_students
user::rwx
user:192525045:rwx
user:192525046:rwx
user:192525047:rwx
user:faculty_advisor:r-x
group::rwx
mask::rwx
other::---
default:user::rwx
default:user:192525045:rwx
default:user:192525046:rwx
default:user:192525047:rwx
default:user:faculty_advisor:r-x
default:group::rwx
default:mask::rwx
default:other::---
```
