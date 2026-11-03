# References

Mechanical design, hall-sensor fingertip and sensor thresholds come from the
InMoov project. Pages read while building this:

- InMoov i2 hand build guide and BOM: https://inmoov.fr/hand-i2/
- InMoov finger sensor how-to (hall sensor + foam versions, threshold idea of
  baseline +1 / +4 / +7): https://inmoov.fr/test-your-finger-sensor/
- InMoov forum thread on adding force feedback to servos with an FSR (position
  nudged forward/back around a min/max force band, PID suggested as a next
  step): https://inmoov.fr/activity/p/12165
- MyRobotLab forum discussion of the AH3503 + magnet sensor range and
  resolution: https://myrobotlab.org/comment/9997
- Another InMoov i2 build with a Mega-based control stack and Python GUI
  (no tactile control): https://github.com/geigermartin/prostheticHand
- InMoov i2 hand driven from OpenCV, with hall-sensor test code:
  https://mc.dfrobot.com.cn/article-382525.html

No code from these projects is copied here. The firmware and Python package are
written for this hardware.
