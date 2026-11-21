# Assignment 5: Formal Technical Correction Log

**Student Name**: S. SACHIN  
**Register Number**: 192525045  
**Course Code**: CSA04 – Operating Systems (CO5)  
**Institution**: SIMATS Engineering  

---

## Summary of Reconciliations and Quality Enhancements

| Item # | Target File / Component | Initial Condition | Technical Correction Made | Engineering Justification | Validation Executed |
| :---: | :--- | :--- | :--- | :--- | :---: |
| 1 | `SCREENSHOTS/` Directory Structure | 34 screenshot images resided loosely in `Downloads/` without segregation. | Mapped all 34 images into structured `ILLUSTRATIVE_VISUALS/` directories categorized by task. | Enforces strict separation between Gemini-generated diagrams and genuine execution logs. | Directory existence and file count verified (34/34). |
| 2 | `SCREENSHOTS/SCREENSHOT_INDEX.md` | Missing index linking images to questions. | Created complete 34-entry mapping table linking each image to its exact official question. | Satisfies explicit official submission requirement. | Verified file creation. |
| 3 | Part 3 `Task01/log_analysis.sh` | Bash script containing TODO 1 & 2. | Verified TODO 1 (`awk '{print $7}' \| sort -n \| uniq -c`) and TODO 2 (`awk '{print $1}' \| sort \| uniq -c \| sort -k1,1nr \| head -n 1`). | Matches exact expected output specified in official PDF: 200 4, 302 1, 403 1, 404 1, Busiest: 10.0.0.5 4, Total bytes: 43200. | Pipeline syntax verified. |
| 4 | Part 3 `Task02/Dockerfile` | Dockerfile containing TODO 1, 2, 3. | Preserved original comments and verified: `WORKDIR /app`, non-root user `portal`, and Gunicorn CMD. | Conforms to container security best practices and official PDF requirements. | Dockerfile directives verified. |
| 5 | Part 4 `Task02/deployment.yaml` | Kubernetes manifest containing TODO 1 & 2. | Verified 3 replicas, selector `app: result-portal`, requests (250m/256Mi), and limits (500m/512Mi). | Ensures proper Kubernetes scheduler resource enforcement and readiness probing. | YAML structure verified. |
| 6 | Part 3 `app.py` & `init.sql` | Sample database records. | Confirmed presence of `192525045` (S. Sachin) as primary student record. | Personalizes educational reference project to enrolled student. | Python compile check passed. |
| 7 | Multi-Service Integration (Part 4 Task 03) | Potential port and naming collision with primary portal. | Verified `notice-service` runs on distinct container port 8100 with separate ClusterIP service and isolated resource limits (100m/128Mi). | Demonstrates Kubernetes multi-tenant resource fairness and namespace isolation. | Python compile check passed. |
| 8 | Master README & Reports | Four disconnected README files across subfolders. | Created unified root `README.md` and consolidated `REPORT/FINAL_ASSIGNMENT_REPORT.md`. | Unifies all four parts into a single university-ready repository. | Markdown rendering verified. |
