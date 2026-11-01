# Calibration

Do this once after assembly and again after re-tensioning any tendon.

## 1. Servo end-stops (per finger)

Run with the tendons slack or the hand unloaded.

1. Start from the narrow defaults in `Config.h` (`FINGER_CAL`).
2. Open the console and sweep one finger: `f 1 0`, `f 1 1`.
3. Widen `openUs` and `closedUs` in small steps (about 25 us) until the finger
   reaches full extension and full flexion. Stop before the servo stalls or
   the line goes slack.
4. If a finger moves the wrong way, swap its two values.
5. Repeat for all five fingers and re-flash.

## 2. Tactile baseline

The baseline is zeroed at power-up. If you power up with a finger loaded, or
temperature has drifted, re-zero with the hand open and unloaded:

    hand> cal

## 3. Contact threshold and grip level

1. `stream 1` in the console, with logging to a CSV.
2. Press each fingertip by hand and note the filtered level when it feels
   firm and when it first registers.
3. Set `CONTACT_ON` just above the idle noise (watch the level at rest).
   Keep `CONTACT_OFF` about half of `CONTACT_ON` for hysteresis.
4. Pick a grip level with `level <counts>` or `grasp <counts>`. Raise it
   until the object holds against a gentle pull, no higher.
5. Bake the values into `Config.h` once happy.

Plot the result with `python tools/plot_tactile.py session.csv`.
