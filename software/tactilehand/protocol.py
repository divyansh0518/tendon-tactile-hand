"""Telemetry frame parsing.

Firmware sends one line per frame:

    T,<ms>,<pos0..pos4>,<press0..press4>,<contact_mask>
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Optional, Tuple

N_FINGERS = 5
FINGER_NAMES = ("thumb", "index", "middle", "ring", "pinky")


@dataclass(frozen=True)
class Frame:
    t_ms: int
    position: Tuple[float, ...]   # % of travel, 0 open .. 100 closed
    press: Tuple[float, ...]      # hall reading above baseline, ADC counts
    contact_mask: int

    def in_contact(self, finger: int) -> bool:
        return bool((self.contact_mask >> finger) & 1)


def parse_frame(line: str) -> Optional[Frame]:
    """Return a Frame, or None if the line is not a valid telemetry frame."""
    parts = line.strip().split(",")
    if len(parts) != 3 + 2 * N_FINGERS or parts[0] != "T":
        return None
    try:
        t_ms = int(parts[1])
        pos = tuple(float(x) for x in parts[2:2 + N_FINGERS])
        press = tuple(float(x) for x in parts[2 + N_FINGERS:2 + 2 * N_FINGERS])
        mask = int(parts[-1])
    except ValueError:
        return None
    return Frame(t_ms, pos, press, mask)
