"""Campus result-portal: a deliberately small Flask application.

Routes
  /         HTML page listing the sample semester results
  /results  the same results as JSON
  /health   liveness/readiness check (used by Docker, Nginx checks and Kubernetes in Part B3)

If DATABASE_URL is set (Docker Compose), results are read from PostgreSQL.
If it is not set (plain `docker run`, systemd demo), built-in sample rows are used.
"""
import os

from flask import Flask, jsonify

app = Flask(__name__)

SAMPLE_RESULTS = [
    {"reg_no": "192525001", "course": "CSA04", "grade": "A"},
    {"reg_no": "192525002", "course": "CSA04", "grade": "B"},
    {"reg_no": "192525045", "course": "CSA04", "grade": "A"},
]


def load_results():
    """Return (rows, source). Falls back to sample rows if the database is unavailable."""
    url = os.environ.get("DATABASE_URL")
    if not url:
        return SAMPLE_RESULTS, "sample-data"
    try:
        import psycopg2

        with psycopg2.connect(url, connect_timeout=3) as conn, conn.cursor() as cur:
            cur.execute("SELECT reg_no, course, grade FROM results ORDER BY reg_no")
            rows = [{"reg_no": r[0], "course": r[1], "grade": r[2]} for r in cur.fetchall()]
        return rows, "postgresql"
    except Exception as exc:  # keep the portal up even if the database is down
        app.logger.warning("database unavailable: %s", exc)
        return SAMPLE_RESULTS, "sample-data (database unavailable)"


@app.route("/")
def index():
    rows, source = load_results()
    body = "".join(
        f"<tr><td>{r['reg_no']}</td><td>{r['course']}</td><td>{r['grade']}</td></tr>" for r in rows
    )
    return (
        "<!doctype html><title>University Result Portal</title>"
        "<h1>University Result Portal</h1>"
        f"<p>Data source: {source}</p>"
        "<table border='1' cellpadding='6'><tr><th>Register No</th><th>Course</th><th>Grade</th></tr>"
        f"{body}</table>"
    )


@app.route("/results")
def results():
    rows, source = load_results()
    return jsonify(source=source, results=rows)


@app.route("/health")
def health():
    return jsonify(status="ok")


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)
