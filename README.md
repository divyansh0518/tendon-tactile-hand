# Tendon-driven tactile hand

A 3D-printed tendon-driven hand with a hall-effect touch sensor in every
fingertip. Each finger is pulled by one 20 kg.cm digital servo through a
braided-line tendon, and the fingertip sensors let the hand stop closing as soon
as it touches something instead of stalling the servo against the object.

<!-- Replace with your own photo -->
![Prototype](docs/images/prototype_final.jpg)


## How it works

- Arduino Mega 2560 runs the control loop: PCA9685 over I2C for the five
  servos, five analog hall sensors on A0-A4.
- Raspberry Pi 4B talks to the Mega over USB serial for control, logging and
  experiments.
- Each fingertip is a molded silicone cap with a small magnet over an AH3503
  linear hall sensor. Pressing the tip moves the magnet closer and shifts the
  sensor output.
- Firmware takes a baseline, filters the readings, detects contact with a
  hysteresis band, and while closing it holds a finger in place once its
  reading passes a configurable limit. Opening is never blocked.

## Hardware

Parts: [hardware/bom.csv](hardware/bom.csv). Wiring: [docs/wiring.md](docs/wiring.md).
Printed parts: [cad/README.md](cad/README.md). Assembly: [docs/assembly.md](docs/assembly.md).

Servos get their own supply with a common ground. Do not run them from the Mega
or the Pi.

## Build and flash

```bash
cd firmware
pio run -e mega -t upload
pio device monitor
```

Before first use set the servo travel for each finger and look at your sensor
readings (see [docs/tuning.md](docs/tuning.md)). The defaults are deliberately
conservative guesses, not tested values.

## Python (on the Pi or a PC)

```bash
cd software
pip install -e .[plot]

python examples/grasp_demo.py /dev/ttyACM0       # COM3 on Windows
python examples/live_plot.py  /dev/ttyACM0
python examples/logger.py     /dev/ttyACM0 -t 20
python examples/calibrate.py  /dev/ttyACM0 index
```

Pi-specific setup: [docs/raspberry_pi.md](docs/raspberry_pi.md).

```python
from tactilehand import Hand

with Hand("/dev/ttyACM0") as hand:
    hand.zero_tactile()
    hand.set_press_limit(8)
    hand.grasp()
```

## Serial protocol

115200 baud, newline-terminated ASCII, commands are case-insensitive.

| Command | Meaning |
|---------|---------|
| `P p0 p1 p2 p3 p4` | Target position per finger, 0 (open) to 100 (closed) |
| `F i pct` | Target for a single finger |
| `OPEN` / `GRASP` | All fingers to 0 / 100 (press-limited) |
| `LIM n` | Press limit, ADC counts above baseline |
| `ZERO` | Re-zero hall baselines (nothing touching the fingertips) |
| `STREAM 0\|1` | Telemetry on/off |

Replies are `OK` or `ERR <reason>`. Telemetry at 25 Hz:

```
T,<ms>,<pos x5>,<press x5>,<contact bitmask>
```

Finger order: thumb, index, middle, ring, pinky.

## Build log

Write this in your own words, with photos.

### Printing
<!-- what you printed, settings, problems -->
![](docs/images/printing_01.jpg)

### Assembly
<!-- joints, tendons, what went wrong -->
![](docs/images/assembly_finger.jpg)
![](docs/images/assembly_palm.jpg)

### Fingertip sensors
<!-- molding the silicone, magnet orientation, sensor soldering -->
![](docs/images/fingertip_sensor.jpg)

### Electronics
![](docs/images/wiring.jpg)

## Results

Add your own measurements here: sensor readings, what objects it can hold,
failure cases. Plots from `logger.py` fit well.

## Known limitations

- Sensor output is in ADC counts above baseline, not newtons, until calibrated.
- The useful hall signal is only a few counts on the Mega's 10-bit ADC.
- Hobby-servo position is open loop; firmware assumes the servo reaches the
  commanded pulse.
- One tendon per finger means no independent joint control.
- Firmware has not been verified on hardware yet. Update this when it has.

## License

Code: MIT, see [LICENSE](LICENSE). Printed parts are not included; see
[cad/README.md](cad/README.md).

Inspired by InMoov.
