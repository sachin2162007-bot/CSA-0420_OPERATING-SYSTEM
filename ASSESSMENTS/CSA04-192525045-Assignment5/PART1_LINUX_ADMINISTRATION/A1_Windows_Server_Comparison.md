# A1 Alternative Concept and Complexity Analysis – Windows Server instead of Linux

**Question (from the assignment):** *If the portal server ran Windows Server instead of Linux, what would the solution be? Evaluate the memory and disk used by the base system, the time to automate administration tasks, and the licence cost per server.*

## 1. Important note on evidence

**No measurements were taken and no prices are invented.** Where figures appear they are vendor-published *minimum requirements* that must be re-checked against current Microsoft and distribution documentation. Everything else is stated as an **assumption** or a **symbolic model**. The real values should be measured on the student's own machines in Part B (`free -h`, `df -h`; Task Manager or `Get-ComputerInfo` on Windows).

## 2. What the Windows Server solution would be

| Portal layer | Linux design | Windows Server equivalent |
|---|---|---|
| Web server / reverse proxy | Nginx | IIS (or Nginx for Windows) |
| App server | Gunicorn + Python (systemd) | Python app via a Windows service (for example a service wrapper) behind IIS/HTTP Platform Handler, or waitress |
| Database | PostgreSQL | PostgreSQL for Windows, or SQL Server (extra licence) |
| Services | `systemd` | Windows Services / Service Control Manager |
| Logs | journald, `/var/log` | Event Log, IIS logs |
| Firewall | `ufw` / netfilter | Windows Defender Firewall |
| Remote admin | SSH with keys | RDP, WinRM/PowerShell remoting, or OpenSSH |
| Automation | bash, cron | PowerShell, Task Scheduler |
| Permissions | rwx, owner/group/others | NTFS ACLs, Active Directory |

The portal would still work; the differences are in resource use, automation style and cost.

## 3. Base system memory and disk

| Criterion | Linux server (text-mode install) | Windows Server | Assessment |
|---|---|---|---|
| Minimum RAM (vendor-stated) | Small; text-mode distributions are documented to run in roughly 1 GB (verify for the chosen distribution) | Microsoft documents a lower minimum for Server Core (about 512 MB) and a higher one with the Desktop Experience (about 2 GB) (verify current release notes) | Both can be small if no GUI is installed. |
| Typical idle memory | Assumption: lower, because no GUI is installed by default | Assumption: higher, especially with Desktop Experience, because more background services run | **To be measured** with `free -h` and Task Manager at idle. |
| Minimum disk (vendor-stated) | A few GB for a minimal install (verify) | About 32 GB documented minimum (verify) | Windows base install is larger. |
| Idle disk used after install | **To be measured** with `df -h` | **To be measured** | Larger base image also means longer backup/restore and more patches. |

**Impact on the portal:** every GB used by the base system is a GB not available for the page cache and for application workers. Memory and disk are therefore a **constant overhead per server**, O(1) per server but O(N) over N servers. A smaller base system means more portal capacity per host and a smaller attack surface. Using Server Core on Windows reduces the gap.

## 4. Time to automate administration

Model the time for N servers as:

- **Linux (bash + cron + SSH):** `T_L(N) = T_write_L + N · t_run_L`
- **Windows (PowerShell + Task Scheduler/WinRM):** `T_W(N) = T_write_W + N · t_run_W`

Both are **O(N)** in the number of servers when applied one by one, and both can be made close to O(N/f) with parallel tools (f = number of parallel connections) or a configuration-management tool.

| Factor | Linux | Windows | Comment |
|---|---|---|---|
| Scripting model | Text streams and pipes; the tools (`awk`, `grep`, `sed`) are small and composable | Object pipeline in PowerShell; strong for structured data and the registry/AD | Different strengths, neither is slower inherently. |
| Remote execution | SSH is built in and standard | WinRM/PowerShell remoting; OpenSSH is now available | Linux path is simpler for a small team. |
| Configuration as text | Almost all settings are text files in `/etc`, easy to copy and version in Git | Many settings in the registry, GUI-driven roles, and AD policies | Text configuration reduces `T_write` and drift. |
| Existing skills | The course and this portal already use Linux | Depends on the staff | Assumption: the team knows Linux; retraining would add to `T_write_W`. |

Conclusion: with the same skills, automation time is of the **same order**; Linux is favoured here by uniform text configuration, SSH and the existing team knowledge. This is a reasoned expectation, not a measurement.

## 5. Licence cost per server

Linux distributions used for this design have **no per-server licence fee** (support is optional). Windows Server is commercially licensed. The general structure (check Microsoft's current licensing terms and price list; no price is given here) is:

`Cost_W(per server) = server licence (based on physical cores, with a minimum core count) + Client Access Licences (CALs) where required + any database licence (for example SQL Server) + support/Software Assurance`

Points for the evaluation:
- The cost grows with **core count and number of servers**, so scaling out for result day multiplies the licence cost: roughly **O(N)** in servers.
- Whether CALs apply to anonymous students over the internet, and whether external-connector licensing is required, depends on Microsoft's terms and must be confirmed with the university's licensing office (the university may hold an academic agreement that changes the cost).
- Linux cost per server for licences: assumed **zero**, but staff time and optional support contracts are real costs.

## 6. Summary and recommendation

| Criterion | Linux | Windows Server | Favoured |
|---|---|---|---|
| Base memory | Expected lower (measure) | Expected higher (measure) | Linux |
| Base disk | Smaller | Larger | Linux |
| Automation effort | Low with existing skills; text config | Comparable with PowerShell; more registry/GUI settings | Linux (assumed) |
| Licence cost/server | No licence fee | Licence required, scales with cores/servers | Linux |
| Fit with Python/Nginx/PostgreSQL/containers | Native | Possible, less common | Linux |
| Fit if the university already runs Active Directory-centred services | Needs integration | Native | Windows |

For this portal, Linux is the better choice mainly because of lower licence cost, lower base overhead and simpler automation. Windows Server would be considered only if the application depended on Windows-only components. These conclusions rest on the stated assumptions and should be updated with measured values.
