#pragma once
#include <Arduino.h>

// Linear hall sensor (AH3503) under a magnet embedded in a silicone fingertip.
// Pressing the tip moves the magnet towards the sensor and shifts the analog
// output. press() is that shift in ADC counts relative to the baseline
// captured by zero(), sign-corrected and clamped at 0. It is a displacement
// proxy, not a calibrated force.
class TactileSensor {
 public:
  void begin(uint8_t pin, int8_t sign, float alpha, uint16_t onCounts,
             uint16_t offCounts);
  void zero();      // call with nothing touching the fingertips
  void update();

  float press() const { return filtered_; }
  bool  contact() const { return contact_; }
  float baseline() const { return baseline_; }

 private:
  float sample() const;

  uint8_t  pin_ = 0;
  int8_t   sign_ = 1;
  float    alpha_ = 0.3f;
  float    baseline_ = 0.0f;
  float    filtered_ = 0.0f;
  uint16_t on_ = 3;
  uint16_t off_ = 2;
  bool     contact_ = false;
};
