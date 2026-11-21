# B3.1 – Virtual-machine deployment  (STATUS: PENDING – not executed)

The sandbox is itself a KVM guest without `/dev/kvm` (no nested virtualisation), so no VM could be started. No VM numbers exist in this
submission and none are estimated. Execute on your own Linux machine:

```bash
# 1. VirtualBox (or KVM/virt-manager): Ubuntu Server 24.04 guest, 1 vCPU, 1024 MB RAM, 10 GB disk, host-only or bridged network
# 2. Inside the guest
sudo apt-get update && sudo apt-get install -y python3-venv
python3 -m venv ~/venv && ~/venv/bin/pip install -r requirements.txt     # same requirements.txt as Part 3
cd <folder containing app.py> && ~/venv/bin/gunicorn --bind 0.0.0.0:8000 app:app
# 3. Measure
#   Start-up : stopwatch from "Start" of the VM until `curl http://<vm-ip>:8000/health` succeeds (boot + service); also record service-only start
#   Memory   : `free -m` inside the guest after start and after load (used column)
#   Disk     : `ls -lh` of the .vdi/.qcow2 file on the host, and `df -h /` inside the guest
#   Requests : from the host:  bash benchmark.sh vm http://<vm-ip>:8000
# 4. Fill the four VM cells in benchmark_results.csv and deployment_comparison.md
```
Use identical `ab -n 5000 -c 50` and the `/results` path so the rows are comparable.
