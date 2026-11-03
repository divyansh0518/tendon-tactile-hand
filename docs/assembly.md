# Assembly notes

Mechanical assembly follows the InMoov i2 hand guide (https://inmoov.fr/hand-i2/).
This page only records what is specific to this build.

## Mechanism (as in i2)

- One braided-line tendon per finger on the palm side, running through a PTFE
  sheath from the finger to the servo pulley.
- One extension spring per finger returns it to the open position when the
  tendon is released.
- Each fingertip is a molded silicone cap with a small magnet in it. A hall
  sensor sits in the printed fingertip behind the cap. Pressing the tip
  moves the magnet toward the sensor.

## Differences from the reference build

Write your own list. Examples of things worth recording:

- Servos: 20 kg.cm digital servos instead of the servo InMoov lists, and what
  you changed so the pulley fits the horn.
- Wrist: left out / included.
- Control: Arduino Mega + PCA9685 driven by this firmware, instead of the
  InMoov NervoBoard + MyRobotLab.

## Hall sensor and magnet

1. Solder the AH3503 to three ribbon wires, insulate the joints with thin tape
   (not thicker than the ribbon, or it won't seat back in the finger).
2. Mold the silicone fingertips with the 2 mm pin in the mold so there is a
   pocket for the magnet.
3. Find the magnet's south pole with a compass and set it into the pocket with
   the south pole facing the sensor. Use a wooden toothpick; metal tools will
   stick to it. Secure with a drop of silicone.
4. Check the reading direction with `live_plot.py`. If pressing the tip makes
   the value go down, set `HALL_SIGN` to -1 in `config.h`.

## Photos

Add photos under `docs/images/` and link them from the README build log.
