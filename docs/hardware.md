# Hardware

## Bill of materials

| Qty | Part | Role |
|---|---|---|
| 6 | High-torque 300 deg servo | 5 finger tendons + 1 wrist rotation |
| 5 | AH3503 linear Hall-effect sensor | fingertip tactile channel |
| 5 | Small magnet | embedded in each fingertip, above its sensor |
| 1 set | Ecoflex silicone | molded compliant fingertips |
| 5 | Braided transmission line, 200 lb rated | tendons |
| 1 set | PTFE guide tubing | low-friction tendon routing |
| 5 | Return spring / elastic | passive finger extension |
| 1 | Arduino-class MCU (Uno/Nano/Mega) | 6 PWM-capable pins, 5 analog inputs |
| 1 | Separate servo supply + bulk capacitor | servo stall current |

The firmware assumes an AVR-style 5 V, 10-bit ADC. Other boards work if you
adjust `Config.h` and re-tune the tactile thresholds (counts scale with ADC
resolution).

## Wiring (matches `Config.h`)

| Signal | Pin |
|---|---|
| Thumb / index / middle / ring / pinky servo | D3 / D5 / D6 / D9 / D10 |
| Wrist servo | D11 |
| Thumb / index / middle / ring / pinky Hall out | A0 / A1 / A2 / A3 / A4 |

- Power the servos from their own supply, never from the MCU 5 V pin. Tie
  grounds together.
- Power the Hall sensors from the same 5 V rail as the ADC reference so the
  ratiometric output tracks the converter. Check the datasheet pin order for
  your AH3503 package before wiring.
- Add a bulk capacitor across the servo rail. Five servos starting together
  can brown out the controller otherwise.

## Mechanical notes

**Actuation.** Each finger has one tendon, anchored at the fingertip, routed
through PTFE guides to a servo horn. The servo flexes the finger by pulling
the line. Extension is passive: a spring returns the finger when the servo
releases. This keeps one actuator per finger and no antagonist line, at the
cost of extension force being fixed by the spring.

**Tactile fingertips.** A magnet is embedded in the Ecoflex tip above an
AH3503. Contact deforms the silicone, the magnet moves relative to the sensor,
and the analog output shifts. The signal tracks indentation, not direction of
load. Mount the sensor and magnet so the deviation is monotonic over the
working range, and keep the same orientation across all five fingers.

**Wrist.** One servo rotates the wrist. Limits are set in `Config.h`.
