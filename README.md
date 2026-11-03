# Tactile-Sensing Tendon-Driven Robotic Hand

A five-finger anthropomorphic hand with tendon-driven fingers, a motorised
wrist, and magnet-and-Hall-effect tactile fingertips. The firmware closes each
finger until it feels the object, then regulates grip from the tactile signal, inspired by inmoov hand.

<!-- Replace with your own media -->
![Hand](media/hand.jpg)

## Highlights

- 6 (20kg/cm) torque 300 deg servos: independent actuation of all five fingers plus
  wrist rotation.
- Tendon transmission: braided 200 lb line through PTFE guides, with
  spring-assisted passive extension (one actuator per finger, no antagonist
  line).
- Tactile fingertips: AH3503 Hall sensors under magnets embedded in molded
  Ecoflex silicone, giving an analog contact signal per finger.
- Tactile-driven grasping: per-finger close-until-contact, then grip regulation
  and slip recovery, all on a 100 Hz loop.

## How it works

```
 host (hand_console.py / plot_tactile.py)
        |  USB serial, 115200 baud
        v
 +----------------------------------------------------------+
 |  MCU                                                     |
 |   command parser --> Hand                                |
 |                       |-- Finger x5  (slew-limited servo)|--> tendons
 |                       |-- Wrist      (slew-limited servo)|--> wrist
 |                       '-- TactileSensor x5                |<-- AH3503
 |                            oversample -> EMA -> baseline |
 |                            -> hysteresis contact flag    |
 +----------------------------------------------------------+
```

### Tactile sensing

Each fingertip is a magnet cast into Ecoflex, sitting above an AH3503. When
the tip is pressed, the silicone deforms, the magnet moves relative to the
sensor, and the analog output shifts. The firmware zeroes against an unloaded
baseline, oversamples (8x), applies an exponential filter, and tracks the
magnitude of the deviation. A hysteresis band (`CONTACT_ON` / `CONTACT_OFF`)
turns that into a stable contact flag.

### Grasp control

Every finger runs its own small state machine:

| State | Behaviour |
|---|---|
| `FREE` | position commanded directly (poses, `f` command) |
| `CLOSING` | flex at a limited speed until the fingertip reports contact |
| `HOLDING` | freeze at contact, then trim tendon travel so the tactile level tracks a set-point |

Tendon travel sets line tension through the servo and return spring, so a
proportional term on tactile error regulates grip without any force sensing at
the actuator. If contact is lost while holding (slip, or the object moves
away), the finger drops back to `CLOSING` and re-engages. Fingers act
independently, so each one settles on the contact geometry it actually meets.

All motion is slew-limited per state, which keeps servo stalls and tendon
shock loads down.

## Repository layout

```
firmware/tendon_hand/
  tendon_hand.ino    serial command interface, scheduler, telemetry
  Config.h           pins, servo calibration, thresholds, gains, poses
  TactileSensor.h    baseline, filtering, contact detection
  Actuators.h        slew-limited finger and wrist servos
  Hand.h             grasp state machine and hand-level commands
tools/
  hand_console.py    interactive serial console + CSV logging
  plot_tactile.py    offline or live plots of travel and tactile signal
docs/
  hardware.md        BOM, wiring, mechanical notes
  calibration.md     servo end-stops, tactile baseline, thresholds
media/               photos, video stills, plots
```


## Limitations

- Finger position is open-loop. The servos have no feedback to the controller,
  so tendon slack or stretch is not observed.
- The tactile channel reports indentation magnitude per fingertip. It does not
  resolve contact location or shear, and it drifts with temperature and
  silicone ageing, hence the re-zero command.
- One tendon per finger with spring return fixes extension force and gives no
  independent joint control within a finger.
- Contact and grip thresholds are tuned by hand per build, in ADC counts, not
  calibrated to physical units.

## Future plans

- Calibrate the Hall signal against a load cell to report force in newtons.
- Add servo-side position or current sensing to close the loop on tendon
  tension.
- Use the five tactile channels for slip detection and in-hand adjustment
  rather than contact-only logic.
- Move to compliant or underactuated finger linkages so the mechanism adapts
  to object shape passively.

## Author

Divyansh Rahangdale

## License

MIT, see [LICENSE](LICENSE).
