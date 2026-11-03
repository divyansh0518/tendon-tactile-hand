#include "TactileSensor.h"
#include "config.h"

void TactileSensor::begin(uint8_t pin, int8_t sign, float alpha,
                          uint16_t onCounts, uint16_t offCounts) {
  pin_ = pin;
  sign_ = sign;
  alpha_ = alpha;
  on_ = onCounts;
  off_ = offCounts;
  pinMode(pin_, INPUT);
  analogRead(pin_);          // throw away the first conversion after mux switch
  zero();
}

float TactileSensor::sample() const {
  uint16_t sum = 0;
  for (uint8_t i = 0; i < HALL_OVERSAMPLE; i++) sum += analogRead(pin_);
  return static_cast<float>(sum) / HALL_OVERSAMPLE;
}

void TactileSensor::zero() {
  float acc = 0.0f;
  for (uint8_t i = 0; i < 32; i++) {
    acc += sample();
    delay(2);
  }
  baseline_ = acc / 32.0f;
  filtered_ = 0.0f;
  contact_ = false;
}

void TactileSensor::update() {
  float v = sign_ * (sample() - baseline_);
  if (v < 0.0f) v = 0.0f;
  filtered_ += alpha_ * (v - filtered_);

  if (!contact_ && filtered_ >= on_)  contact_ = true;
  if (contact_  && filtered_ <= off_) contact_ = false;
}
