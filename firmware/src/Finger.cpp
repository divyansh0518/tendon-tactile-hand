#include "Finger.h"

void Finger::begin(Adafruit_PWMServoDriver *driver, uint8_t channel,
                   uint16_t openUs, uint16_t closedUs, float maxSpeedPctPerS) {
  driver_   = driver;
  ch_       = channel;
  openUs_   = openUs;
  closedUs_ = closedUs;
  maxSpeed_ = maxSpeedPctPerS;
  pos_ = target_ = 0.0f;
  write();
}

void Finger::setTarget(float pct) {
  if (pct < 0.0f)   pct = 0.0f;
  if (pct > 100.0f) pct = 100.0f;
  target_ = pct;
}

void Finger::update(float dt, bool blocked) {
  float diff = target_ - pos_;
  if (diff > 0.0f && blocked) return;   // pushing into contact: hold position

  float step = maxSpeed_ * dt;
  if (diff >  step) diff =  step;
  if (diff < -step) diff = -step;
  if (diff == 0.0f) return;

  pos_ += diff;
  write();
}

void Finger::write() {
  float us = openUs_ + (static_cast<float>(closedUs_) - openUs_) * (pos_ / 100.0f);
  driver_->writeMicroseconds(ch_, static_cast<uint16_t>(us + 0.5f));
}
