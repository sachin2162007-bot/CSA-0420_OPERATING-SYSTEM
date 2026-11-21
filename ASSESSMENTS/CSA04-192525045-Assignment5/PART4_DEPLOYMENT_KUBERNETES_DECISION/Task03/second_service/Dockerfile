# Container image for the notice service (second service, B4.1). Same pattern as the Part 3 portal image.
FROM python:3.12-slim
WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
RUN useradd --create-home --shell /usr/sbin/nologin notice
COPY --chown=notice:notice app.py .
USER notice
EXPOSE 8100
CMD ["gunicorn", "--bind", "0.0.0.0:8100", "app:app"]
