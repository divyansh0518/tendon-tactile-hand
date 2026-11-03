"""Live fingertip hall-sensor plot. Needs the [plot] extra (matplotlib)."""
import argparse
from collections import deque

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

from tactilehand import Hand, FINGER_NAMES, N_FINGERS

WINDOW = 250  # frames (~5 s at 50 Hz)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    args = ap.parse_args()

    hand = Hand(args.port)
    buf = [deque([0.0] * WINDOW, maxlen=WINDOW) for _ in range(N_FINGERS)]

    fig, ax = plt.subplots()
    lines = [ax.plot(range(WINDOW), list(b), label=n)[0]
             for b, n in zip(buf, FINGER_NAMES)]
    ax.set_ylim(0, 40)
    ax.set_xlabel("frame")
    ax.set_ylabel("press (ADC counts above baseline)")
    ax.legend(loc="upper left", ncol=5)

    def update(_):
        for f in hand.poll_frames():
            for i in range(N_FINGERS):
                buf[i].append(f.press[i])
        for line, b in zip(lines, buf):
            line.set_ydata(list(b))
        return lines

    anim = FuncAnimation(fig, update, interval=40, blit=False,
                         cache_frame_data=False)
    try:
        plt.show()
    finally:
        hand.close()
    return anim


if __name__ == "__main__":
    main()
