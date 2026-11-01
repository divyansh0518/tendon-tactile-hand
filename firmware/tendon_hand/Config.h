// Config.h - pin map, servo calibration and control gains.
// Everything hardware-specific lives here. Edit this file, not the logic.
#pragma once
#include <Arduino.h>

namespace cfg {

// ---------------------------------------------------------------- layout
constexpr uint8_t NUM_FINGERS = 5;
enum FingerId : uint8_t { THUMB, INDEX, MIDDLE, RING, PINKY };
constexpr const char* FINGER_NAME[NUM_FINGERS] = {"thumb", "index", "middle", "ring", "pinky"};

// ---------------------------------------------------------------- pins
// Written for an Arduino Uno/Nano/Mega-class board (5 V ADC, 10-bit).
constexpr uint8_t FINGER_SERVO_PIN[NUM_FINGERS] = {3, 5, 6, 9, 10};
constexpr uint8_t WRIST_SERVO_PIN = 11;
constexpr uint8_t TACTILE_PIN[NUM_FINGERS] = {A0, A1, A2, A3, A4};

// ---------------------------------------------------------------- servos
// 300 deg servos: 500..2500 us spans the full travel.
constexpr uint16_t SERVO_MIN_US = 500;
constexpr uint16_t SERVO_MAX_US = 2500;
constexpr float SERVO_RANGE_DEG = 300.0f;

// Pulse width at fully extended (0.0) and fully flexed (1.0) finger.
// openUs > closedUs is fine; it just flips the direction for that finger.
// These defaults are deliberately narrow. Calibrate before trusting them
// (see docs/calibration.md).
struct FingerCal { uint16_t openUs; uint16_t closedUs; };
constexpr FingerCal FINGER_CAL[NUM_FINGERS] = {
    {1200, 1800},  // thumb
    {1200, 1800},  // index
    {1200, 1800},  // middle
    {1200, 1800},  // ring
    {1200, 1800},  // pinky
};

constexpr float WRIST_CENTER_DEG = 150.0f;
constexpr float WRIST_MIN_DEG = 60.0f;
constexpr float WRIST_MAX_DEG = 240.0f;

// ---------------------------------------------------------------- timing
constexpr uint32_t BAUD = 115200;
constexpr uint16_t CONTROL_PERIOD_MS = 10;    // 100 Hz control loop
constexpr uint16_t TELEMETRY_PERIOD_MS = 20;  // 50 Hz when streaming

// ---------------------------------------------------------------- motion
// Slew limits, in fraction of finger travel per second.
constexpr float FREE_SLEW = 2.0f;    // commanded poses
constexpr float CLOSE_SLEW = 0.6f;   // approaching an object
constexpr float HOLD_SLEW = 0.25f;   // force regulation while in contact
constexpr float WRIST_SLEW_DPS = 120.0f;

// ---------------------------------------------------------------- tactile
// All thresholds are in ADC counts of deviation from the unloaded baseline.
constexpr uint8_t BASELINE_SAMPLES = 64;
constexpr uint8_t OVERSAMPLE = 8;
constexpr float EMA_ALPHA = 0.25f;
constexpr float CONTACT_ON = 12.0f;   // enter contact above this
constexpr float CONTACT_OFF = 6.0f;   // leave contact below this (hysteresis)

// ---------------------------------------------------------------- grasp
constexpr float GRASP_LEVEL_DEFAULT = 40.0f;  // tactile set-point while holding
constexpr float HOLD_KP = 0.01f;              // travel/s per count of error
constexpr float HOLD_DEADBAND = 3.0f;         // counts

// ---------------------------------------------------------------- poses
// Finger travel per pose: 0 = extended, 1 = flexed.
struct Pose { const char* name; float pos[NUM_FINGERS]; };
static const Pose POSES[] = {
    {"open",     {0.00f, 0.00f, 0.00f, 0.00f, 0.00f}},
    {"fist",     {1.00f, 1.00f, 1.00f, 1.00f, 1.00f}},
    {"point",    {1.00f, 0.00f, 1.00f, 1.00f, 1.00f}},
    {"peace",    {1.00f, 0.00f, 0.00f, 1.00f, 1.00f}},
    {"thumbsup", {0.00f, 1.00f, 1.00f, 1.00f, 1.00f}},
    {"pinch",    {0.70f, 0.60f, 0.00f, 0.00f, 0.00f}},
};
constexpr uint8_t NUM_POSES = sizeof(POSES) / sizeof(POSES[0]);

}  // namespace cfg
