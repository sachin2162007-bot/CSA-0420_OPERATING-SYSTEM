# Part 1 – Linux Administration (A1 and A2)

| | |
|---|---|
| **Name** | S. Sachin |
| **Register Number** | 192525045 |
| **Course** | CSA04 – Operating Systems |
| **Degree** | B.Tech Artificial Intelligence and Machine Learning |
| **Institution** | SIMATS Engineering |
| **Assignment** | Assignment 5 – CO5: Linux, Containers and Virtualisation for Efficient Operations |
| **Allocation** | A1 (7 marks) and A2 (5 marks) = 12 marks |
| **Source** | Official PDF `ASST_QUESTIONS_NEW-pages-5.pdf` (only authority for requirements) |

Case study: deploying the University Result Portal (150,000 students in two hours on result day).

## Contents

| Official question | Marks | File |
|---|---|---|
| A1.1 Linux Architecture | 2 | [A1_Linux_Architecture.md](A1_Linux_Architecture.md) |
| A1.2 Process and Memory Management | 3 | [A1_Process_Memory_Management.md](A1_Process_Memory_Management.md) |
| A1.3 File System and Permissions | 2 | [A1_Filesystem_Permissions.md](A1_Filesystem_Permissions.md) |
| A1 Alternative: Windows Server | – | [A1_Windows_Server_Comparison.md](A1_Windows_Server_Comparison.md) |
| A2.1 Users, Services and Logs | 2 | [A2_User_Service_Log_Design.md](A2_User_Service_Log_Design.md) |
| A2.2 Networking Services | 2 | [A2_Network_Architecture.md](A2_Network_Architecture.md) |
| A2.3 Automation | 1 | [A2_Automation_Plan.md](A2_Automation_Plan.md) |
| A2 Alternative: Ansible | – | [A2_Ansible_Alternative.md](A2_Ansible_Alternative.md) |
| Big-O summary | – | [complexity_analysis.md](complexity_analysis.md) |
| Sources | – | [references.md](references.md) |
| Diagrams D1–D10 (supporting) | – | [diagrams/](diagrams/) |

## Notes on scope and honesty

- This is a **design and analysis** part. No Docker, Kubernetes or virtualisation is implemented here, and no terminal output or measurement is claimed.
- Configuration snippets (systemd, Nginx, ufw, sshd, cron, Ansible) are **design sketches**, not tested output. Addresses and domain names are placeholders.
- Figures from vendors (Windows Server minimums) are labelled as vendor-stated and must be re-verified. No licence price is stated.
- Diagrams are written in Mermaid and render on GitHub.
- The official PDF refers to the *SIMATS Engineering – Standard for Algorithmic Pseudocode* but does not include it; the only pseudocode (disk alert) uses a plain structured style and should be adjusted to the standard.

## Git commit suggestion (assignment requires progressive commits)

One commit per task: `A1.1`, `A1.2`, `A1.3`, `A1 alternative`, `A2.1`, `A2.2`, `A2.3`, `A2 alternative`, `diagrams`, `references`.
