# B3.1, B3.2, B3.4 – Deploy three ways, measure, trade-off

Student: S. Sachin | Register number: 192525045 | CSA04 Operating Systems | B.Tech AI and ML, SIMATS Engineering

Same application everywhere: the Part 3 Flask result portal (`PART3_LINUX_DOCKER_IMPLEMENTATION/Task02/app.py`), started with the same command
`gunicorn --bind <addr>:<port> app:app` (one sync worker, gunicorn default).

| Deployment | Status | Evidence |
|---|---|---|
| Bare metal (Linux venv) | ACTUAL, 3 runs | `benchmark_results.csv`, `bare_metal_deployment.md` |
| Container (Part 3 image) | ACTUAL, 3 runs | `benchmark_results.csv`, `container_deployment.md` |
| Virtual machine | PENDING (no `/dev/kvm` in the sandbox) | `vm_deployment.md` gives the exact procedure |

Files: `benchmark.sh` (re-runnable), `benchmark_results.csv`, `measurement_methodology.md` (hardware, versions, caveats), `deployment_comparison.md` (B3.4).
Run: `bash benchmark.sh bare`, `bash benchmark.sh container`, `bash benchmark.sh vm http://<vm-ip>:8000`.
