from __future__ import annotations

import time
from typing import Iterator, List, Optional, Sequence

import serial

from .protocol import Frame, N_FINGERS, parse_frame


class HandError(RuntimeError):
    pass


class Hand:
    """Thin wrapper around the firmware's line protocol."""

    def __init__(self, port: str, baud: int = 115200, timeout: float = 0.1):
        self.ser = serial.Serial(port, baud, timeout=timeout)
        time.sleep(2.0)                    # Mega auto-resets when the port opens
        self.ser.reset_input_buffer()
        self.last_frame: Optional[Frame] = None

    # -- context manager --------------------------------------------------
    def __enter__(self) -> "Hand":
        return self

    def __exit__(self, *exc) -> None:
        self.close()

    def close(self) -> None:
        if self.ser.is_open:
            try:
                self.ser.write(b"OPEN\n")
            finally:
                self.ser.close()

    # -- commands ---------------------------------------------------------
    def _cmd(self, text: str, timeout: float = 1.0) -> None:
        self.ser.write((text + "\n").encode("ascii"))
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            raw = self.ser.readline().decode("ascii", errors="ignore").strip()
            if not raw or raw.startswith("#"):
                continue
            frame = parse_frame(raw)
            if frame is not None:          # telemetry interleaved with replies
                self.last_frame = frame
                continue
            if raw == "OK":
                return
            if raw.startswith("ERR"):
                raise HandError(f"{text!r} -> {raw}")
        raise HandError(f"no reply to {text!r}")

    def set_positions(self, pct: Sequence[float]) -> None:
        if len(pct) != N_FINGERS:
            raise ValueError(f"need {N_FINGERS} values")
        self._cmd("P " + " ".join(f"{v:.1f}" for v in pct))

    def set_finger(self, index: int, pct: float) -> None:
        self._cmd(f"F {index} {pct:.1f}")

    def open(self) -> None:
        self._cmd("OPEN")

    def grasp(self) -> None:
        self._cmd("GRASP")

    def set_press_limit(self, counts: float) -> None:
        self._cmd(f"LIM {counts:.0f}")

    def zero_tactile(self) -> None:
        self._cmd("ZERO", timeout=2.0)

    def stream(self, enabled: bool) -> None:
        self._cmd(f"STREAM {int(enabled)}")

    # -- telemetry --------------------------------------------------------
    def read_frame(self) -> Optional[Frame]:
        """Blocking read of one line (up to the port timeout)."""
        raw = self.ser.readline().decode("ascii", errors="ignore")
        frame = parse_frame(raw)
        if frame is not None:
            self.last_frame = frame
        return frame

    def poll_frames(self) -> List[Frame]:
        """Return every frame already waiting in the buffer, without blocking."""
        out = []
        while self.ser.in_waiting:
            frame = self.read_frame()
            if frame is not None:
                out.append(frame)
        return out

    def frames(self) -> Iterator[Frame]:
        while True:
            frame = self.read_frame()
            if frame is not None:
                yield frame
