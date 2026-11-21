# Task A1.3 – Linux File System and Permissions (2 Marks)

## 1. Directory Hierarchy

Linux uses a **single tree** rooted at `/`. Additional disks and file systems are *mounted* onto directories in this tree instead of receiving drive letters. The layout follows the Filesystem Hierarchy Standard (FHS); see [Diagram D5](diagrams/directory_hierarchy.md).

| Directory | Purpose | Relevance to the portal |
|---|---|---|
| `/` | Root of the tree | |
| `/bin`, `/sbin`, `/lib` | Essential programs and libraries (often symbolic links to `/usr`) | System tools |
| `/usr` | Installed software and shared read-only data | Python, Nginx binaries |
| `/etc` | System-wide configuration files | `nginx/`, `systemd/system/`, `ssh/sshd_config`, `ufw/` |
| `/var/log` | Log files | `nginx/access.log`, `journal/` |
| `/var/lib` | Persistent program state | PostgreSQL data directory |
| `/home` | User home directories | Administrators' keys and scripts |
| `/root` | Superuser's home | |
| `/tmp`, `/run` | Temporary and runtime files | Gunicorn socket |
| `/dev` | Device files | Disks (`sda`) |
| `/proc`, `/sys` | Virtual kernel interfaces | Monitoring |
| `/srv`, `/opt` | Served data and optional packages | Portal code (design choice: `/srv/portal`) |
| `/mnt`, `/media` | Mount points | Backup volume |

Mounting parts of the tree on separate partitions (for example `/var/log` and the database area) stops a runaway log from filling the root file system.

## 2. Virtual File System (VFS)

The VFS is the kernel layer that gives applications **one interface to all file systems** ([Diagram D4](diagrams/vfs_layers.md)). A program calls `open()`, `read()` or `write()`; the VFS finds the file-system driver responsible for that path and passes the request on.

Main VFS objects:
- **Superblock:** describes a mounted file system.
- **Inode:** describes one file (type, owner, permission bits, size, timestamps, location of data) but not its name.
- **Dentry:** a cached link between a name and an inode; the *dentry cache* makes path lookup fast, roughly O(d) for a path with d components when entries are cached.
- **File object:** an open instance of a file, holding the current offset and access mode.

Benefits: the same tools (`cp`, `cat`) work on ext4, XFS, tmpfs, NFS, `/proc` and `/sys`; new file systems can be added as modules; and the page cache sits behind the VFS for all of them. For the portal, this means application code is independent of whether data is on a local disk or a network mount.

## 3. Owner, Group and Others

Every file has an **owner (user)** and a **group**. Access rights are given to three classes: the **owner (u)**, members of the **group (g)**, and **others (o)**. The kernel checks the classes in that order and applies the *first matching class* ([Diagram D6](diagrams/permission_model.md)); it does not combine them.

## 4. File Permissions

Each class has three bits: **read (r=4)**, **write (w=2)**, **execute (x=1)**.

| Bit | On a file | On a directory |
|---|---|---|
| r | Read contents | List names |
| w | Modify contents | Create, delete, rename entries |
| x | Run as a program | Enter (`cd`) and access items by name |

Example: `-rw-r----- 1 portal portal-app app.conf` means mode **640**: owner `portal` may read and write, group `portal-app` may read, others have no access. Commands: `chmod 640 file`, `chown portal:portal-app file`, `ls -l`. The default for new files is set by `umask` (for example `umask 027` removes all rights for others).

Other special bits: **setgid** on a directory makes new files inherit the directory's group (useful for a shared deployment directory); the **sticky bit** (`/tmp`) lets only a file's owner delete it.

Least-privilege example for the portal:

| Item | Owner:Group | Mode | Reason |
|---|---|---|---|
| `/srv/portal` (code) | `deploy:portal-app` | 750 | Service reads, only deployers write |
| `/etc/portal/portal.env` (DB password) | `root:portal-app` | 640 | Secrets not readable by others |
| `/var/log/portal` | `portal:adm` | 750 | Admins can read logs |

## 5. setuid

The **setuid** bit (mode `4xxx`, shown as `s` in the owner execute position) makes a program run with the privileges of its *file owner* rather than of the user who started it. The kernel therefore distinguishes the **real UID** from the **effective UID**. A well-known example is `passwd`, which is owned by root so an ordinary user can change their entry in the protected password database. (`setgid`, mode `2xxx`, does the same for groups.)

Risk: a bug in a setuid-root program can give an attacker root access. Good practice for the portal server: keep the number of setuid programs small (audit with `find / -perm -4000 -type f`), never make scripts or the portal application setuid, and mount data partitions with `nosuid`.

## 6. sudo

`sudo` lets an authorised user run a specific command as another user (normally root), after authenticating with their *own* password. It is itself a setuid-root program, controlled by `/etc/sudoers` (edited with `visudo`) and drop-in files in `/etc/sudoers.d/`.

Advantages over logging in as root: (1) every privileged command is **logged** with the user's name; (2) rights can be **limited to commands**, for example allowing the `operators` group only `systemctl restart portal`; (3) no shared root password; and (4) privileges last only for the command. Example rule: `%operators ALL=(root) /usr/bin/systemctl restart portal`. Direct root login over SSH is disabled in the design ([A2_Network_Architecture.md](A2_Network_Architecture.md)).
