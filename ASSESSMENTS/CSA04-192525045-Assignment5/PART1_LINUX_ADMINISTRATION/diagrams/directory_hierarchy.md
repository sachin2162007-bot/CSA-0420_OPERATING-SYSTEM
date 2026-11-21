# Diagram D5 – Linux Directory Hierarchy and Portal Placement (supports A1.3)

```text
/                         root of the single directory tree
├── bin, sbin, lib        essential programs and libraries (often symlinks into /usr)
├── boot                  kernel image, bootloader files
├── dev                   device files (sda, null, tty)
├── etc                   configuration  -> nginx/, systemd/, ufw/, ssh/
├── home                  user home directories
├── proc                  virtual: processes and kernel data
├── sys                   virtual: devices and drivers
├── root                  home of the superuser
├── run, tmp              runtime state, temporary files
├── usr                   installed software, shared data
├── opt                   optional packages
├── srv                   data served by the system
│   └── portal/           (design) application code and static files
├── var
│   ├── log               logs  -> nginx/, journal/, portal/
│   ├── lib               variable state -> postgresql/
│   ├── backups           (design) local backup staging
│   └── spool             cron and queues
└── mnt, media            mount points
```
