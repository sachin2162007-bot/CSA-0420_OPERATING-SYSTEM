"""Notice service: a second, separate service that runs next to the result portal (B4.1).
Routes: /  /notices (JSON)  /health"""
from flask import Flask, jsonify

app = Flask(__name__)
NOTICES = [
    {"id": 1, "text": "Semester results are published on the result portal."},
    {"id": 2, "text": "Online examination timetable is available in the LMS."},
]


@app.route("/")
def index():
    return "<h1>University Notice Service</h1>" + "".join(f"<p>{n['text']}</p>" for n in NOTICES)


@app.route("/notices")
def notices():
    return jsonify(notices=NOTICES)


@app.route("/health")
def health():
    return jsonify(status="ok")


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8100)
