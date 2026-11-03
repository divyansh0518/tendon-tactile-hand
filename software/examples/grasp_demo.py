"""Close the hand until the fingertips reach the press limit, hold, then release."""
import argparse
import time

from tactilehand import Hand, FINGER_NAMES


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    ap.add_argument("--limit", type=float, default=8, help="press limit (ADC counts above baseline)")
    ap.add_argument("--hold", type=float, default=2.0, help="seconds to hold")
    args = ap.parse_args()

    with Hand(args.port) as hand:
        hand.zero_tactile()
        hand.set_press_limit(args.limit)
        hand.open()
        time.sleep(1.0)

        hand.grasp()
        t0 = time.time()
        while time.time() - t0 < args.hold + 1.5:
            hand.poll_frames()
            time.sleep(0.05)

        f = hand.last_frame
        if f:
            for i, name in enumerate(FINGER_NAMES):
                print(f"{name:7s} pos={f.position[i]:5.1f}%  press={f.press[i]:6.1f}  "
                      f"contact={f.in_contact(i)}")
        hand.open()
        time.sleep(1.0)


if __name__ == "__main__":
    main()
