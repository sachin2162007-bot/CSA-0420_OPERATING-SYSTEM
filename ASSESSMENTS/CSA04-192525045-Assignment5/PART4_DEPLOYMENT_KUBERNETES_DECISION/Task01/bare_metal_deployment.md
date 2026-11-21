# B3.1 – Bare-metal deployment (directly on Linux)

Host: Ubuntu 24.04.5, Python 3.12.3, 1 vCPU, 3992 MiB (see `measurement_methodology.md`).

```bash
python3 -m venv /tmp/portal-venv
/tmp/portal-venv/bin/pip install -r PART3_LINUX_DOCKER_IMPLEMENTATION/Task02/requirements.txt   # flask 3.1.3, gunicorn 27.0.1, psycopg2-binary
cd PART3_LINUX_DOCKER_IMPLEMENTATION/Task02
/tmp/portal-venv/bin/gunicorn --bind 127.0.0.1:8001 app:app
curl http://127.0.0.1:8001/health        # {"status":"ok"}
```
`benchmark.sh bare` performs exactly this (3 start/load/stop cycles). Without `DATABASE_URL` the app serves its built-in sample rows,
identically in all three deployments. Disk figure = size of the venv plus `app.py` (system Python not counted).
