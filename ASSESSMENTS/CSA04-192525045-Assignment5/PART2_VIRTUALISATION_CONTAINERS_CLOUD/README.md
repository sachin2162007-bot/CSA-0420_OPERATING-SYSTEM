# Part 2 – Virtualisation, Containers and Cloud (A3, A4 and A5)

| | |
|---|---|
| **Name** | S. Sachin |
| **Register Number** | 192525045 |
| **Course** | CSA04 – Operating Systems |
| **Degree** | B.Tech Artificial Intelligence and Machine Learning |
| **Institution** | SIMATS Engineering |
| **Assignment** | Assignment 5 – CO5: Linux, Containers and Virtualisation for Efficient Operations |
| **Allocation** | A3 (5 marks), A4 (4 marks) and A5 (4 marks) = 13 marks |
| **Source** | Official PDF `ASST_QUESTIONS_NEW-pages-5.pdf` (only authority for requirements) |

Case study: deploying the University Result Portal (150,000 students in two hours on result day). Part 1 (A1 and A2) is in `PART1_LINUX_ADMINISTRATION/` and is not repeated here.

## Contents

| Official question | Marks | File |
|---|---|---|
| A3.1 Hypervisors | 3 | [A3_Hypervisors.md](A3_Hypervisors.md) |
| A3.2 Benefits and Costs | 2 | [A3_Virtualisation_Benefits_Costs.md](A3_Virtualisation_Benefits_Costs.md) |
| A3 Alternative: VirtualBox (type-2) instead of KVM (type-1) | – | [A3_Type1_vs_Type2_Analysis.md](A3_Type1_vs_Type2_Analysis.md) |
| A4.1 How Containers Work | 2 | [A4_Container_Architecture.md](A4_Container_Architecture.md) |
| A4.2 Containers versus Virtual Machines | 2 | [A4_Containers_vs_Virtual_Machines.md](A4_Containers_vs_Virtual_Machines.md) |
| A4 Alternative: portal and database in VMs | – | [A4_VM_Alternative_Analysis.md](A4_VM_Alternative_Analysis.md) |
| A5.1 Cloud Set-up | 2 | [A5_Cloud_Deployment_Design.md](A5_Cloud_Deployment_Design.md) |
| A5.2 Kubernetes | 2 | [A5_Kubernetes_Architecture.md](A5_Kubernetes_Architecture.md) |
| A5 Alternative: fixed VMs instead of autoscaling | – | [A5_Fixed_VM_vs_Autoscaling.md](A5_Fixed_VM_vs_Autoscaling.md) |
| Big-O summary | – | [complexity_analysis.md](complexity_analysis.md) |
| Sources | – | [references.md](references.md) |
| Diagrams D11–D16 (supporting) | – | [diagrams/](diagrams/) |

## Notes on scope and honesty

- This is a **design and analysis** part. No Docker application, VM or Kubernetes cluster is created, and no command output, benchmark or measurement is claimed.
- **No cloud or licence prices are stated.** Cost is given as formulas in VM-hours. The single worked example in `A5_Fixed_VM_vs_Autoscaling.md` (section 6) uses **hypothetical parameters** to show the method and is labelled as such.
- Start-up, overhead and memory comparisons are qualitative orders of magnitude or symbolic models; values are to be measured in Part B (B3.2, B4.2).
- Licensing and product-edition statements (VMware, VirtualBox extensions, Hyper-V) change over time and must be re-verified.
- Diagrams are written in Mermaid and render on GitHub; numbering D11–D16 continues from Part 1 (D1–D10).
- The official PDF refers to the *SIMATS Engineering – Standard for Algorithmic Pseudocode* but does not include it; the pseudocode in the A4 alternative uses a plain structured style and should be adjusted to the standard.
- Terminology follows Part 1: `portal-web`, `portal-db`, Nginx, Gunicorn, PostgreSQL, `results.example.edu`, the DMZ/internal subnets.

## Integration with Part 1 and Part B

1. Place this folder beside `PART1_LINUX_ADMINISTRATION/` in the Git repository `CSA04-192525045-Assignment5`.
2. Merge `references.md` and `complexity_analysis.md` into the final report as separate sections for Part 1 and Part 2 (they are kept separate so Part 1 is unchanged).
3. In Part B, replace "measure" cells using B3.2 (start-up time, memory, image size, requests per second) and B4.2 (capacity), and add the provider's prices for the cost question in B4.3.
4. Link the Part B2 Dockerfile and Part B3 `deployment.yaml` to the concepts in A4.1 and A5.2.

## Git commit suggestion (progressive commits are required)

One commit per task: `A3.1`, `A3.2`, `A3 alternative`, `A4.1`, `A4.2`, `A4 alternative`, `A5.1`, `A5.2`, `A5 alternative`, `diagrams`, `complexity and references`.
