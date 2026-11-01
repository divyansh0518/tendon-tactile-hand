#!/usr/bin/env python3
"""Interactive serial console for the tendon_hand firmware.

    python tools/hand_console.py --port /dev/ttyACM0 --log session.csv

Anything you type is sent to the board. Telemetry lines (after `stream 1`) are
written to the log file; everything else the board says is printed.
"""
import argparse
import threading
import time

import serial


def reader(ser, log, stop):
    while not stop.is_set():
        try:
            raw = ser.readline()
        except serial.SerialException:
            break
        if not raw:
            continue
        line = raw.decode(errors="replace").strip()
        if not line:
            continue
        if line.startswith("t_ms") or line[0].isdigit():
            if log:
                log.write(line + "\n")
        else:
            print(line)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
    ap.add_argument("--port", required=True)
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--log", help="CSV file for telemetry")
    args = ap.parse_args()

    ser = serial.Serial(args.port, args.baud, timeout=0.2)
    time.sleep(2.0)  # most Arduino boards reset on connect
    log = open(args.log, "w", buffering=1) if args.log else None

    stop = threading.Event()
    t = threading.Thread(target=reader, args=(ser, log, stop), daemon=True)
    t.start()
    ser.write(b"help\n")

    try:
        while True:
            cmd = input("hand> ").strip()
            if cmd in ("quit", "exit"):
                break
            if cmd:
                ser.write((cmd + "\n").encode())
    except (EOFError, KeyboardInterrupt):
        pass
    finally:
        ser.write(b"stream 0\n")
        stop.set()
        t.join(timeout=1)
        ser.close()
        if log:
            log.close()


if __name__ == "__main__":
    main()
