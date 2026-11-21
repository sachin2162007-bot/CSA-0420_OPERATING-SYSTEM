# Alternative Concept and Complexity Analysis (B1 and B2)

Labels: **MEASURED** = run in this session (1 vCPU sandbox); **REASONED** = derived from how the tools work, not measured; **PENDING** = needs a real multi-server or Docker-capable environment. No ELK or Docker Hub measurement was possible here.

## Part 1 – B1: local shell-script log analysis vs a central log system (journald + log server, or ELK)

### What the alternative looks like
Every server forwards its logs (journald → `systemd-journal-remote`, or Filebeat/rsyslog → Logstash → Elasticsearch → Kibana) to one place; errors are found by one query instead of logging in to each server.

```text
LOCAL (implemented)                      CENTRAL (alternative)
for each server s in 1..N:               ship logs continuously to collector
    ssh s "log_analysis / grep ERROR"    index once on arrival
merge results by hand                    query: status:500 AND @timestamp:[now-1h TO now]
```

### Comparison
| Criterion | Local scripts (implemented) | Central logging (journald server or ELK) |
|---|---|---|
| **Time to find an error across 10 servers** | One search per server: about 0.06 s of `grep` per 1,000,000-line (55 MB) log (**MEASURED**); 10 servers searched one after another about 10 × that plus ssh set-up time per host. The machine time is trivial; **the human cost (logging in 10 times, merging output) dominates** (**REASONED**). With `ssh` in parallel the wall time is about one server's time plus network | One query over an index; typically seconds, independent of N for the user. Cost moves to ingest time (small delay before a log line is searchable) (**REASONED**, **PENDING**: not measured) |
| **Extra storage** | None beyond the logs (plus rotation) | A second full copy of all logs, plus index overhead. Elasticsearch indexes commonly make stored data larger than the raw text, so plan for a multiple of the raw volume; the factor depends on mapping and compression and must be measured (**PENDING**) |
| **Extra memory** | The script used a peak of about 11.6 MB RSS on 1,000,000 lines (**MEASURED**), only while it runs | A permanent service: Elasticsearch is a JVM application and needs gigabytes of heap on the log server, plus agents (tens of MB each) on every server (**REASONED**, vendor sizing guides apply) |
| **Set-up effort** | Minutes: copy two scripts, add cron (done in B1.2) | Hours to days: install, secure and size the cluster, write parsers, dashboards, retention rules; journald+log-server is lighter than ELK but still needs TLS and a collector (**REASONED**) |
| **Other** | Logs lost if the server dies or is compromised; no cross-server correlation | Logs survive a failed server, enable correlation and alerting; one more system to fail and secure |

### Decision
For one portal server, local scripts are enough, which is why Part A kept them. Central logging becomes worthwhile when logs must survive server loss, when about 10 or more servers or containers exist, or on result day when errors must be correlated across many instances. Containers (B2/B3) make this stronger because their logs disappear with the container.

## Part 2 – B2: full Ubuntu base vs python:3.12-slim vs multi-stage

### What each alternative looks like
`Dockerfile` (official, slim) / `Dockerfile.multistage` (wheels built in stage 1, copied to a fresh slim stage) / `Dockerfile.ubuntu` (ubuntu:24.04 + apt python3). All are in `Task02/`.

### Comparison (REASONED; every number PENDING)
| Criterion | Ubuntu 24.04 base | python:3.12-slim (official) | Multi-stage |
|---|---|---|---|
| Image size | Largest: full distribution userland plus apt-installed Python | Smaller: minimal Debian with Python already included | About equal to slim for this app, because all dependencies are pre-built wheels (nothing heavy to discard) |
| Build time | Longest: `apt-get update` and Python install on every cold build | Shorter; dependency layer cached | A little longer cold (two stages), same warm |
| Pull time to a new server | Proportional to compressed bytes, so largest | Smaller | About the same as slim |
| Number of OS packages (attack surface) | Most | Fewer | Same as slim |
| Potential security problems | More packages means more CVEs to track, patch and scan | Fewer | Same as slim; benefit appears only if build tools would otherwise be in the final image |
| Other | Familiar tools, more debugging convenience | Official Python image maintained by Docker | Extra file to maintain |

Measurement table and commands: `Task02/image_optimisation.md`; `capture_evidence.sh` fills it. **PENDING.**
