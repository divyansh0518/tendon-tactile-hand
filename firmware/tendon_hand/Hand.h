// Hand.h - five fingers, five tactile channels, one wrist, and the grasp logic.
//
// Per-finger grasp state machine:
//
//   FREE     position is commanded directly (poses, `f` command)
//   CLOSING  flex at CLOSE_SLEW until the fingertip reports contact
//   HOLDING  freeze, then trim tendon travel so the filtered tactile level
//            tracks a set-point. If contact is lost (slip), go back to CLOSING.
//
// Tendon travel maps to line tension through the servo and the return spring,
// so a small P-term on tactile error is enough to regulate grip without a
// force sensor on the servo itself.
#pragma once
#include <string.h>
#include "Config.h"
#include "TactileSensor.h"
#include "Actuators.h"

class Hand {
 public:
  enum Mode : uint8_t { FREE, CLOSING, HOLDING };

  void begin() {
    for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) {
      fingers_[i].begin(cfg::FINGER_SERVO_PIN[i], cfg::FINGER_CAL[i]);
      sensors_[i].begin(cfg::TACTILE_PIN[i]);
      mode_[i] = FREE;
    }
    wrist_.begin(cfg::WRIST_SERVO_PIN);
  }

  void update(float dt) {
    for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) sensors_[i].update();

    for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) {
      switch (mode_[i]) {
        case FREE:
          break;
        case CLOSING:
          if (sensors_[i].contact()) {
            fingers_[i].setTarget(fingers_[i].position());  // freeze at contact
            fingers_[i].setSlew(cfg::HOLD_SLEW);
            mode_[i] = HOLDING;
          }
          break;
        case HOLDING: {
          if (!sensors_[i].contact()) {  // object slipped or was removed
            fingers_[i].setSlew(cfg::CLOSE_SLEW);
            fingers_[i].setTarget(1.0f);
            mode_[i] = CLOSING;
            break;
          }
          float err = graspLevel_ - sensors_[i].level();
          if (fabsf(err) > cfg::HOLD_DEADBAND)
            fingers_[i].setTarget(fingers_[i].target() + cfg::HOLD_KP * err * dt);
          break;
        }
      }
    }

    for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) fingers_[i].update(dt);
    wrist_.update(dt);
  }

  // ---- commands
  void setFinger(uint8_t i, float p) {
    mode_[i] = FREE;
    fingers_[i].setSlew(cfg::FREE_SLEW);
    fingers_[i].setTarget(p);
  }

  void open() {
    for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) setFinger(i, 0.0f);
  }

  void grasp() {
    for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) {
      mode_[i] = CLOSING;
      fingers_[i].setSlew(cfg::CLOSE_SLEW);
      fingers_[i].setTarget(1.0f);
    }
  }

  bool pose(const char* name) {
    for (uint8_t k = 0; k < cfg::NUM_POSES; k++) {
      if (strcmp(name, cfg::POSES[k].name) == 0) {
        for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) setFinger(i, cfg::POSES[k].pos[i]);
        return true;
      }
    }
    return false;
  }

  void setWrist(float deg) { wrist_.set(deg); }
  void setGraspLevel(float counts) { graspLevel_ = max(counts, cfg::CONTACT_ON + 2.0f); }
  void recalibrateTactile() { for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) sensors_[i].calibrate(); }

  // ---- accessors
  const Finger& finger(uint8_t i) const { return fingers_[i]; }
  const TactileSensor& sensor(uint8_t i) const { return sensors_[i]; }
  Mode mode(uint8_t i) const { return mode_[i]; }
  float wristDeg() const { return wrist_.deg(); }
  float graspLevel() const { return graspLevel_; }

 private:
  Finger fingers_[cfg::NUM_FINGERS];
  TactileSensor sensors_[cfg::NUM_FINGERS];
  Wrist wrist_;
  Mode mode_[cfg::NUM_FINGERS];
  float graspLevel_ = cfg::GRASP_LEVEL_DEFAULT;
};
