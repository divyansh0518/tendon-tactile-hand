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


## Build log


### Printing
<!-- what you printed, settings, problems -->
![](docs/images/printing_03.jpg)
![](docs/images/printing_01.jpg)

Parts used (i2 hand, one hand)
i2_CoverFingerV3, i2_FingersMoldX5V3, i2_FingersTipX5V2, i2_FingersX5V1, i2_HandCoverV1, i2_PalmCoverV2, i2_PulleyX5V1, i2_WristGearV1, i2_WristLargeV2

Print settings i used
Fill in what you actually used (InMoov suggests 0.25 mm max layer height, 30% infill, 2 mm walls, supports only on the cover / tip parts):

Printer: Bambu lab h2d
Material: Pla + and ABS
Layer height: 0.16
Infill / walls: 55% / 4

### Assembly
<!-- joints, tendons, what went wrong -->
![](docs/images/assembly_finger2.jpg)
![](docs/images/assembly_palm.jpg)

### Fingertip sensors
<!-- molding the silicone, magnet orientation, sensor soldering -->
![](docs/images/fingertip_sensor.jpg)

### Fixing Servos

![](docs/images/tendons_spool.jpg)


## Current Limitations

- Finger position is open-loop. The servos have no feedback to the controller,
  so tendon slack or stretch is not observed.
- The tactile channel reports indentation magnitude per fingertip. It does not
  resolve contact location or shear, and it drifts with temperature and
  silicone ageing, hence the re-zero command.
- One tendon per finger with spring return fixes extension force and gives no
  independent joint control within a finger.
- Contact and grip thresholds are tuned by hand per build, in ADC counts, not
  calibrated to physical units.

## Future Goles

- Calibrate the Hall signal against a load cell to report force in newtons.
- Add servo-side position or current sensing to close the loop on tendon
  tension.
- Use the five tactile channels for slip detection and in-hand adjustment
  rather than contact-only logic.
- Move to compliant or underactuated finger linkages so the mechanism adapts
  to object shape passively.



