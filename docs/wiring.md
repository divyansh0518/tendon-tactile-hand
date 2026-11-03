# Wiring

## Overview

```
 Raspberry Pi 4B  --USB-->  Arduino Mega 2560  --I2C-->  PCA9685  --> 5 servos
   (host, Python)               |   A0..A4                  ^
                                |                           |
                          5 x AH3503 hall sensors     servo supply (separate)
```

The Pi only talks to the Mega over USB serial. All timing-critical work (servo
pulses, sensor sampling, stopping a finger on contact) happens on the Mega.

## Power

- Servos run from their own supply into the PCA9685 V+ terminal, with a
  1000 uF capacitor across it. Use the voltage and current the servo
  datasheet gives; 20 kg.cm digital servos can pull a few amps each when
  stalled, so size the supply for five stalling at once and use thick wire.
- Servo supply ground, PCA9685 GND and Mega GND must be common.
- Do not power servos from the Mega or the Pi.
- Pi: its own official 5 V / 3 A supply. The Mega can be powered over the USB
  link from the Pi.

## Mega <-> PCA9685

| Mega | PCA9685 |
|------|---------|
| 20 (SDA) | SDA |
| 21 (SCL) | SCL |
| 5V | VCC (logic) |
| GND | GND |

Servo n plugs into PCA9685 channel n (thumb = 0 ... pinky = 4).

## Hall sensors

AH3503, three pins: supply, ground, output. Check the pin order against the
datasheet for your package before soldering; the InMoov page also warns that
one side is positive and the other negative.

| Finger | Mega pin |
|--------|----------|
| Thumb  | A0 |
| Index  | A1 |
| Middle | A2 |
| Ring   | A3 |
| Pinky  | A4 |

Supply from the Mega's 5 V, output straight to the analog pin. This is the same
finger order the InMoov sensor board uses. Change `HALL_PIN` in
`firmware/include/config.h` if yours differs.

## Noise

- The signal swing is only a few ADC counts. Keep sensor leads away from
  servo power wires, and twist or at least bundle each sensor's three wires.
- Servo motors contain magnets and carry current. Keep the hall sensors and
  their wires away from them and re-zero after the hand is assembled.
