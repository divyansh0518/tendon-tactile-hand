#pragma once
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// One tendon, one servo. Position is 0..100 (% of spool travel),
// 0 = open, 100 = fully closed.
class Finger {
 public:
  void begin(Adafruit_PWMServoDriver *driver, uint8_t channel,
             uint16_t openUs, uint16_t closedUs, float maxSpeedPctPerS);

  void setTarget(float pct);
  float target() const { return target_; }
  float position() const { return pos_; }

  // dt in seconds. If `blocked` is true the finger will not advance in the
  // closing direction (used when the fingertip sensor reports too much press).
  void update(float dt, bool blocked);

 private:
  void write();

  Adafruit_PWMServoDriver *driver_ = nullptr;
  uint8_t  ch_ = 0;
  uint16_t openUs_ = 1500;
  uint16_t closedUs_ = 1500;
  float    maxSpeed_ = 200.0f;
  float    pos_ = 0.0f;
  float    target_ = 0.0f;
};
