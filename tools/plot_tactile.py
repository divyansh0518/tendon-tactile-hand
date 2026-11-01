#!/usr/bin/env python3
"""Plot hand telemetry, offline from a CSV log or live from the board.

    python tools/plot_tactile.py session.csv --out session.png
    python tools/plot_tactile.py --live /dev/ttyACM0
"""
import argparse
import csv
import time
from collections import deque

import matplotlib.pyplot as plt
import numpy as np

FINGERS = ["thumb", "index", "middle", "ring", "pinky"]
COLUMNS = (["t_ms"] + [f"pos_{f}" for f in FINGERS] + ["wrist_deg"]
           + [f"tac_{f}" for f in FINGERS] + [f"contact_{f}" for f in FINGERS])


def load(path):
    with open(path, newline="") as fh:
        rows = [r for r in csv.DictReader(fh) if r.get("t_ms", "").isdigit()]
    data = {k: np.array([float(r[k]) for r in rows]) for k in COLUMNS}
    data["t"] = (data["t_ms"] - data["t_ms"][0]) / 1000.0
    return data


def draw(axs, d):
    for ax in axs:
        ax.clear()
    for f in FINGERS:
        axs[0].plot(d["t"], d[f"pos_{f}"], label=f)
        axs[1].plot(d["t"], d[f"tac_{f}"], label=f)
    axs[0].set_ylabel("tendon travel (0 open, 1 flexed)")
    axs[1].set_ylabel("tactile |deviation| (ADC counts)")
    axs[1].set_xlabel("time (s)")
    axs[0].legend(ncol=5, fontsize=8, loc="upper right")
    for ax in axs:
        ax.grid(alpha=0.3)


def offline(path, out):
    d = load(path)
    fig, axs = plt.subplots(2, 1, sharex=True, figsize=(9, 6))
    draw(axs, d)
    fig.tight_layout()
    if out:
        fig.savefig(out, dpi=150)
        print(f"saved {out}")
    else:
        plt.show()


def live(port, baud, window_s):
    import serial
    ser = serial.Serial(port, baud, timeout=0.05)
    time.sleep(2.0)
    ser.write(b"stream 1\n")

    n = int(window_s * 50)  # firmware streams at 50 Hz
    buf = {k: deque(maxlen=n) for k in COLUMNS}
    fig, axs = plt.subplots(2, 1, sharex=True, figsize=(9, 6))
    plt.ion()
    t0 = None
    try:
        while plt.fignum_exists(fig.number):
            while ser.in_waiting:
                parts = ser.readline().decode(errors="replace").strip().split(",")
                if len(parts) != len(COLUMNS) or not parts[0].isdigit():
                    continue
                for k, v in zip(COLUMNS, parts):
                    buf[k].append(float(v))
            if len(buf["t_ms"]) > 2:
                d = {k: np.array(v) for k, v in buf.items()}
                t0 = d["t_ms"][0] if t0 is None else t0
                d["t"] = (d["t_ms"] - d["t_ms"][0]) / 1000.0
                draw(axs, d)
            plt.pause(0.05)
    finally:
        ser.write(b"stream 0\n")
        ser.close()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
    ap.add_argument("log", nargs="?", help="CSV log from hand_console.py")
    ap.add_argument("--out", help="save figure instead of showing it")
    ap.add_argument("--live", metavar="PORT", help="plot straight from the serial port")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--window", type=float, default=10.0, help="live window, seconds")
    a = ap.parse_args()
    if a.live:
        live(a.live, a.baud, a.window)
    elif a.log:
        offline(a.log, a.out)
    else:
        ap.error("give a log file or --live PORT")


if __name__ == "__main__":
    main()
