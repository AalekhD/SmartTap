import time
from collections import deque
from datetime import datetime
from threading import Lock

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

HISTORY_LENGTH = 300
TELEMETRY_FIELDS = ("uptime_ms", "flow_lpm", "total_litres", "distance_mm", "tof_ok", "tof_valid")

_lock = Lock()
_latest = None
_latest_ts = 0.0
_history = deque(maxlen=HISTORY_LENGTH)


@app.post("/api/telemetry")
def receive_telemetry():
    global _latest, _latest_ts
    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return jsonify(error="expected a JSON object"), 400

    now = time.time()
    record = {key: data.get(key) for key in TELEMETRY_FIELDS}
    record["device"] = str(data.get("device", "unknown"))
    record["received_at"] = datetime.fromtimestamp(now).strftime("%H:%M:%S")

    with _lock:
        _latest = record
        _latest_ts = now
        _history.append(record)

    return jsonify(status="ok")


@app.get("/api/history")
def get_history():
    with _lock:
        latest = dict(_latest) if _latest else None
        latest_ts = _latest_ts
        rows = list(_history)

    if latest:
        latest["age_s"] = round(time.time() - latest_ts, 1)
    return jsonify(latest=latest, history=rows)


@app.get("/")
def dashboard():
    return render_template("dashboard.html")


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
