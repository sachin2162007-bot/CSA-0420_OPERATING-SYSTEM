# A2 Alternative Concept and Complexity Analysis – Ansible instead of Hand-written Shell Scripts

**Question:** *If the server were configured with a configuration-management tool such as Ansible instead of hand-written shell scripts, what would the solution be? Evaluate the time to set up 1 server and 20 servers, the risk of differences between servers, and the effort to learn and maintain each approach.* ([Diagram D10](diagrams/ansible_vs_shell.md))

## 1. Evidence note

No timings were measured. Times are expressed with **symbols** and compared as orders of growth; qualitative ratings are reasoned assumptions to be confirmed when the student actually builds the server in Part B.

## 2. What the Ansible solution would be

Ansible is agentless: a control machine connects to each server over SSH and runs **tasks** described declaratively in YAML **playbooks**, grouped into **roles**. A list of servers is kept in an **inventory**. Tasks are **idempotent**: they describe the desired state ("package nginx is present", "file has mode 0640", "service portal is enabled and started"), and running the playbook again changes nothing if the server already matches.

Example structure (design sketch, not executed):

```text
inventory.ini           # [portal_web] ... [portal_db] ...
site.yml                # which roles apply to which group
roles/
  users/      tasks: groups, accounts, ssh keys, sudoers
  nginx/      tasks: install, template config, enable service
  portal/     tasks: code, systemd unit, service
  firewall/   tasks: ufw rules
  automation/ tasks: copy scripts, cron entries, logrotate
```

```yaml
- name: Nginx is installed and running
  hosts: portal_web
  become: true
  tasks:
    - ansible.builtin.apt: { name: nginx, state: present }
    - ansible.builtin.template: { src: portal.conf.j2, dest: /etc/nginx/sites-available/portal }
      notify: reload nginx
  handlers:
    - ansible.builtin.service: { name: nginx, state: reloaded }
```

The shell scripts for backup and alerts can still be deployed *by* Ansible, so the approaches are complementary: Ansible handles **configuration and distribution**, scripts and cron handle **runtime tasks**.

## 3. Time model

Let `T_w` = time to write the automation, `t` = time to apply it to one server, `N` = number of servers, `f` = parallel connections (Ansible `forks`, default small number such as 5).

| Approach | Total time | Growth |
|---|---|---|
| Hand-written scripts, run server by server | `T_w(script) + N · (t_copy + t_run + t_check)` | **O(N)** |
| Scripts distributed with an SSH loop | `T_w(script) + N · t_run` | **O(N)** (no parallelism unless coded) |
| Ansible playbook | `T_w(playbook) + ceil(N / f) · t_run` | **O(N / f)**, which is O(N) with a smaller constant |

Authoring effort `T_w` does **not** grow with N for either approach (O(1) in servers), but the *manual* part does for scripts.

| Criterion | Shell scripts | Ansible | Reasoning |
|---|---|---|---|
| Set up **1 server** | **Shorter**: writing a few commands and a script is quick; no tool to install | Longer: install Ansible, write inventory, roles and templates | For one server the extra structure is overhead. |
| Set up **20 servers** | Grows with N: copy, run and verify on each; errors repeat | Written once, applied to all with `ansible-playbook`; about N/f batches | Break-even occurs at a small number of servers (a few). |
| Re-configure after a change | Edit and re-copy to every server | Edit the playbook once and re-run | Single source of truth. |

## 4. Risk of differences between servers (configuration drift)

| Aspect | Shell scripts | Ansible |
|---|---|---|
| Same final state on all servers | Depends on whether every script ran everywhere and in the same order | Declared state is enforced on every run |
| Re-running safely | Scripts may fail or duplicate actions (for example appending a cron line twice) unless carefully written | Idempotent modules are safe to repeat |
| Manual hot-fixes on one server | Remain unnoticed and cause drift | Detected with check mode (`--check --diff`) and corrected on the next run |
| Documentation | The scripts are the only record, if kept up to date | Playbooks act as readable, version-controlled documentation |
| Overall drift risk | **High and growing with N** | **Low** |

For result day, where several identical servers must behave identically, drift is a serious risk: one server with a different Nginx or `ulimit` setting can fail under load.

## 5. Learning effort

| Aspect | Shell scripts | Ansible |
|---|---|---|
| Prerequisites | Linux command line, bash, cron | Linux and SSH, plus YAML, Jinja2 templates, module names, inventory, roles |
| Learning time | Lower for someone already using Linux (the course teaches it) | Higher initially; learning curve for idempotent style and variables |
| Debugging | Direct, but must handle every error manually | Structured output; need to understand module behaviour |
| Team knowledge | Widely known | Common in industry, so skill is reusable |

## 6. Maintenance effort

| Aspect | Shell scripts | Ansible |
|---|---|---|
| Per-change work | O(N) manual distribution, or custom deployment code | O(1) edit + one run |
| Code quality | Grows complex as error handling, ordering and OS differences are added | Modules hide OS differences and handle errors |
| Testing | Manual | Dry run (`--check`), test servers, repeatable runs |
| Dependency | None beyond the OS | Control machine, Ansible version, SSH access |
| Space/memory | Negligible | Control node needs Ansible and Python; targets need Python; the small agentless footprint is O(1) per target |

## 7. Conclusion and recommendation

| Situation | Better choice |
|---|---|
| A single server, a small one-off task | Shell scripts and cron |
| Several identical servers, result-day scaling, need for repeatability | **Ansible** (with the existing backup and alert scripts deployed by it) |

Hand-written scripts are quicker to produce and learn for the first server, but their cost and risk of inconsistency grow roughly linearly with the number of servers. Ansible costs more to learn and set up once, then keeps all servers identical with O(N/f) apply time and O(1) maintenance edits. For the official Part B tasks, shell scripts are sufficient; for a real production fleet the recommended approach is a hybrid.
