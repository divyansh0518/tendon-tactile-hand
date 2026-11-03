#pragma once
#include <Arduino.h>

// ---- Hardware --------------------------------------------------------------
// Arduino Mega 2560 + PCA9685 (I2C: SDA = 20, SCL = 21 on the Mega).
constexpr uint8_t  NUM_FINGERS = 5;            // thumb, index, middle, ring, pinky
constexpr uint8_t  PCA_ADDR    = 0x40;
constexpr uint32_t PCA_OSC_HZ  = 27000000UL;   // measure yours if servos jitter
constexpr float    PWM_FREQ_HZ = 50.0f;        // digital servos: check the
                                               // datasheet, some accept more

// PCA9685 channel for each finger's servo
constexpr uint8_t SERVO_CH[NUM_FINGERS] = {0, 1, 2, 3, 4};

// AH3503 hall sensor outputs. Same finger order as the InMoov sensor board
// (thumb = 0 ... pinky = 4).
constexpr uint8_t HALL_PIN[NUM_FINGERS] = {A0, A1, A2, A3, A4};

// +1 if the reading rises when the fingertip is pressed (magnet south pole
// towards the sensor, as in the InMoov i2 build). Use -1 if you mounted the
// magnet the other way round and the reading drops.
constexpr int8_t HALL_SIGN = +1;

// ---- Servo travel (microseconds) ---------------------------------------------
// "Open" = tendon released, springs hold the finger extended.
// "Closed" = tendon fully wound on the pulley. Tune every finger separately,
// and swap the two numbers of a finger if its servo turns the wrong way.
// Start narrow and widen slowly: over-travel stalls a 20 kg servo against the
// tendon and can snap the line or strip the printed pulley.
constexpr uint16_t PULSE_OPEN_US[NUM_FINGERS]   = {1000, 1000, 1000, 1000, 1000};
constexpr uint16_t PULSE_CLOSED_US[NUM_FINGERS] = {2000, 2000, 2000, 2000, 2000};

// ---- Motion ------------------------------------------------------------------
constexpr float MAX_SPEED_PCT_S = 150.0f;      // slew limit, % of travel per second

// ---- Tactile -----------------------------------------------------------------
// The AH3503 is a linear (analog) hall sensor that idles near mid-supply.
// Readings are ADC counts relative to a baseline taken at boot / ZERO, and the
// useful swing is small (InMoov's own thresholds are baseline +1 / +4 / +7),
// so everything below is in single-digit counts. Plot your own sensors first
// (software/examples/live_plot.py) and then adjust.
constexpr uint8_t  HALL_OVERSAMPLE      = 8;      // analogReads averaged per sample
constexpr float    TACTILE_ALPHA        = 0.30f;  // EMA filter, 1.0 = no filtering
constexpr uint16_t CONTACT_ON_COUNTS    = 3;
constexpr uint16_t CONTACT_OFF_COUNTS   = 2;      // hysteresis
constexpr float    DEFAULT_PRESS_LIMIT  = 8.0f;   // closing stops at/above this

// ---- Loop timing / serial ----------------------------------------------------
constexpr uint32_t SERIAL_BAUD       = 115200;
constexpr uint16_t CONTROL_PERIOD_MS = 20;     // 50 Hz (5 sensors x 8 reads ~ 5 ms)
constexpr uint16_t STREAM_PERIOD_MS  = 40;     // 25 Hz telemetry
constexpr uint8_t  RX_BUF_SIZE       = 64;
