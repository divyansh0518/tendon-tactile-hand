// Actuators.h - slew-limited servo wrappers for the tendon fingers and wrist.
#pragma once
#include <Arduino.h>
#include <Servo.h>
#include <math.h>
#include "Config.h"

// A finger is one servo pulling one tendon against a return spring.
// Position is normalised: 0 = extended, 1 = flexed.
class Finger {
 public:
  void begin(uint8_t pin, cfg::FingerCal cal) {
    cal_ = cal;
    servo_.attach(pin, cfg::SERVO_MIN_US, cfg::SERVO_MAX_US);
    pos_ = target_ = 0;
    lastUs_ = 0;
    write();
  }

  void setTarget(float p) { target_ = constrain(p, 0.0f, 1.0f); }
  void setSlew(float s) { slew_ = s; }

  void update(float dt) {
    float step = slew_ * dt;
    float err = target_ - pos_;
    if (fabsf(err) <= step) pos_ = target_;
    else pos_ += (err > 0) ? step : -step;
    write();
  }

  float position() const { return pos_; }
  float target() const { return target_; }

 private:
  void write() {
    int32_t span = (int32_t)cal_.closedUs - (int32_t)cal_.openUs;
    uint16_t us = (uint16_t)((int32_t)cal_.openUs + (int32_t)(span * pos_));
    if (us != lastUs_) {  // skip redundant writes, less servo chatter
      servo_.writeMicroseconds(us);
      lastUs_ = us;
    }
  }

  Servo servo_;
  cfg::FingerCal cal_{1200, 1800};
  float pos_ = 0, target_ = 0, slew_ = cfg::FREE_SLEW;
  uint16_t lastUs_ = 0;
};

class Wrist {
 public:
  void begin(uint8_t pin) {
    servo_.attach(pin, cfg::SERVO_MIN_US, cfg::SERVO_MAX_US);
    deg_ = target_ = cfg::WRIST_CENTER_DEG;
    lastUs_ = 0;
    write();
  }

  void set(float deg) { target_ = constrain(deg, cfg::WRIST_MIN_DEG, cfg::WRIST_MAX_DEG); }

  void update(float dt) {
    float step = cfg::WRIST_SLEW_DPS * dt;
    float err = target_ - deg_;
    if (fabsf(err) <= step) deg_ = target_;
    else deg_ += (err > 0) ? step : -step;
    write();
  }

  float deg() const { return deg_; }

 private:
  void write() {
    float f = deg_ / cfg::SERVO_RANGE_DEG;
    uint16_t us = cfg::SERVO_MIN_US + (uint16_t)(f * (cfg::SERVO_MAX_US - cfg::SERVO_MIN_US));
    if (us != lastUs_) {
      servo_.writeMicroseconds(us);
      lastUs_ = us;
    }
  }

  Servo servo_;
  float deg_ = cfg::WRIST_CENTER_DEG, target_ = cfg::WRIST_CENTER_DEG;
  uint16_t lastUs_ = 0;
};
