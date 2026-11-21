# B4.1 – Practical extension: a second service next to the result portal

`notice-service` (Flask, own image `simats/notice-service:1.0`, port 8100, non-root user `notice`) is a separate Deployment, labels and Service from `result-portal`.
It stands in for the shared university workload (notices for the LMS/exam system); no other features were added.

Files: `second_service/` (app.py, requirements.txt, Dockerfile), `second_service_deployment.yaml`, `second_service_service.yaml`, `practical_extension.md`, `actual_output.txt`.
Build/deploy: `docker build -t simats/notice-service:1.0 second_service` → load into the cluster → `kubectl apply -f second_service_deployment.yaml -f second_service_service.yaml`.
