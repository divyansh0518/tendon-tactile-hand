// TactileSensor.h - one AH3503 Hall sensor under a magnet-in-silicone fingertip.
//
// The AH3503 is a ratiometric linear Hall sensor: its output sits near Vcc/2
// with no field and moves with flux density. When the Ecoflex tip deforms, the
// embedded magnet moves relative to the sensor and the output shifts. We
// zero against an unloaded baseline and track the magnitude of the deviation,
// which is monotonic with indentation but does not encode direction.
#pragma once
#include <Arduino.h>
#include <math.h>
#include "Config.h"

class TactileSensor {
 public:
  void begin(uint8_t pin) {
    pin_ = pin;
    pinMode(pin_, INPUT);
    calibrate();
  }

  // Re-zero. The fingertip must be unloaded.
  void calibrate() {
    float acc = 0;
    for (uint8_t i = 0; i < cfg::BASELINE_SAMPLES; i++) {
      acc += readOversampled();
      delay(2);
    }
    baseline_ = acc / cfg::BASELINE_SAMPLES;
    level_ = 0;
    contact_ = false;
  }

  void update() {
    float dev = fabsf(readOversampled() - baseline_);
    level_ += cfg::EMA_ALPHA * (dev - level_);
    if (!contact_ && level_ > cfg::CONTACT_ON) contact_ = true;
    else if (contact_ && level_ < cfg::CONTACT_OFF) contact_ = false;
  }

  float level() const { return level_; }      // filtered |deviation|, ADC counts
  bool contact() const { return contact_; }
  float baseline() const { return baseline_; }

 private:
  float readOversampled() {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < cfg::OVERSAMPLE; i++) sum += analogRead(pin_);
    return sum / (float)cfg::OVERSAMPLE;
  }

  uint8_t pin_ = 0;
  float baseline_ = 512;
  float level_ = 0;
  bool contact_ = false;
};
