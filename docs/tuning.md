# Tuning and calibration

## Servo travel

1. Keep the conservative defaults in `config.h` (1000-2000 us) to start.
2. Move one finger at a time and watch the tendon:
   ```
   F 1 0
   F 1 100
   ```
3. Widen `PULSE_OPEN_US` / `PULSE_CLOSED_US` for that finger until it is fully
   extended at 0 and fully curled at 100, with no humming/stall at either end.
   Swap the two numbers if the servo turns the wrong way.
4. Repeat for each finger. They will differ.

## Hall sensors

Resting values depend on the sensor and on what is nearby, and the swing is small
(a handful of counts on the Mega's 10-bit ADC). A sensible first pass:

1. Run `python examples/live_plot.py <port>` with the hand untouched and note
   the noise level.
2. Press each fingertip lightly, then firmly, and note the readings.
3. Set `CONTACT_ON_COUNTS` clearly above the noise, `CONTACT_OFF_COUNTS` a bit
   below it, and `DEFAULT_PRESS_LIMIT` at the reading you want a finger to
   stop at. `LIM <n>` changes the limit at runtime without reflashing.

The baseline is captured at boot and on `ZERO`. Nothing should touch the
fingertips at that moment, and the hand should be in its normal pose.

## Counts to grams

Use `examples/calibrate.py` with known masses on the fingertip. The response of
a magnet-in-silicone sensor is non-linear and differs per finger, so calibrate
each finger with its final silicone cap on.

If the resolution is too coarse for what you want, an external 16-bit ADC
(e.g. ADS1115 on I2C) would give a lot more resolution than the Mega's 10-bit
ADC; that's not in the firmware yet.
