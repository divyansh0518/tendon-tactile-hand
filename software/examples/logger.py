"""Log telemetry to CSV while you drive the hand from another terminal / script."""
import argparse
import csv
import time

from tactilehand import Hand, FINGER_NAMES


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    ap.add_argument("-o", "--out", default=f"../logs/run_{int(time.time())}.csv")
    ap.add_argument("-t", "--seconds", type=float, default=30.0)
    args = ap.parse_args()

    header = (["t_ms"] + [f"pos_{n}" for n in FINGER_NAMES]
              + [f"press_{n}" for n in FINGER_NAMES] + ["contact_mask"])

    with Hand(args.port) as hand, open(args.out, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(header)
        end = time.time() + args.seconds
        n = 0
        while time.time() < end:
            f = hand.read_frame()
            if f is None:
                continue
            w.writerow([f.t_ms, *f.position, *f.press, f.contact_mask])
            n += 1
        print(f"wrote {n} frames to {args.out}")


if __name__ == "__main__":
    main()
