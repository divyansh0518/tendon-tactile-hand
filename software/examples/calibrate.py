"""Map one fingertip's counts to grams using known weights.

Put a known mass on the fingertip pad when prompted, press Enter, and the
script averages a short burst of frames. A power-law fit (counts = a * g^b)
is printed at the end. A hall sensor + magnet in silicone is not linear (field falls
off with distance, silicone is not a perfect spring), so don't assume it is.
"""
import argparse
import csv

import numpy as np

from tactilehand import Hand, FINGER_NAMES


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    ap.add_argument("finger", choices=FINGER_NAMES)
    ap.add_argument("--weights", type=float, nargs="+",
                    default=[20, 50, 100, 200, 500], help="grams")
    ap.add_argument("--frames", type=int, default=50)
    ap.add_argument("-o", "--out", default=None)
    args = ap.parse_args()

    idx = FINGER_NAMES.index(args.finger)
    out = args.out or f"../logs/cal_{args.finger}.csv"
    rows = []

    with Hand(args.port) as hand:
        input("Nothing on the sensor, then press Enter to zero... ")
        hand.zero_tactile()
        for grams in args.weights:
            input(f"Place {grams:g} g on the {args.finger} pad, press Enter... ")
            vals = []
            while len(vals) < args.frames:
                f = hand.read_frame()
                if f:
                    vals.append(f.press[idx])
            mean = float(np.mean(vals))
            print(f"  {grams:g} g -> {mean:.1f} counts (sd {np.std(vals):.1f})")
            rows.append((grams, mean))

    with open(out, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["grams", "counts"])
        w.writerows(rows)

    g = np.array([r[0] for r in rows])
    c = np.array([r[1] for r in rows])
    ok = c > 0
    if ok.sum() >= 2:
        b, log_a = np.polyfit(np.log(g[ok]), np.log(c[ok]), 1)
        print(f"fit: counts = {np.exp(log_a):.3f} * g^{b:.3f}")
    print(f"saved {out}")


if __name__ == "__main__":
    main()
