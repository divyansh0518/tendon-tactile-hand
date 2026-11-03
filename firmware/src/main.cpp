// Tendon-driven tactile hand - firmware
// Arduino Mega 2560 + PCA9685 + 5 digital servos + 5 fingertip hall sensors.
// Serial protocol is documented in the top-level README.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "config.h"
#include "Finger.h"
#include "TactileSensor.h"

static Adafruit_PWMServoDriver pwm(PCA_ADDR);
static Finger fingers[NUM_FINGERS];
static TactileSensor tactile[NUM_FINGERS];

static float pressLimit = DEFAULT_PRESS_LIMIT;
static bool  streaming  = true;

static char    rx[RX_BUF_SIZE];
static uint8_t rxLen = 0;

static uint32_t lastControl = 0;
static uint32_t lastStream  = 0;

// ---------------------------------------------------------------------------

static void sendFrame(uint32_t now) {
  uint8_t mask = 0;
  Serial.print(F("T,"));
  Serial.print(now);
  for (uint8_t i = 0; i < NUM_FINGERS; i++) {
    Serial.print(',');
    Serial.print(fingers[i].position(), 1);
  }
  for (uint8_t i = 0; i < NUM_FINGERS; i++) {
    Serial.print(',');
    Serial.print(tactile[i].press(), 1);
    if (tactile[i].contact()) mask |= (1 << i);
  }
  Serial.print(',');
  Serial.println(mask);
}

static void setAll(float pct) {
  for (uint8_t i = 0; i < NUM_FINGERS; i++) fingers[i].setTarget(pct);
}

static void handleLine(char *line) {
  char *save = nullptr;
  char *cmd = strtok_r(line, " ", &save);
  if (!cmd) return;

  if (!strcmp(cmd, "P")) {                       // P p0 p1 p2 p3 p4
    float v[NUM_FINGERS];
    for (uint8_t i = 0; i < NUM_FINGERS; i++) {
      char *tok = strtok_r(nullptr, " ", &save);
      if (!tok) { Serial.println(F("ERR args")); return; }
      v[i] = atof(tok);
    }
    for (uint8_t i = 0; i < NUM_FINGERS; i++) fingers[i].setTarget(v[i]);
    Serial.println(F("OK"));

  } else if (!strcmp(cmd, "F")) {                // F idx pct
    char *a = strtok_r(nullptr, " ", &save);
    char *b = strtok_r(nullptr, " ", &save);
    if (!a || !b) { Serial.println(F("ERR args")); return; }
    int idx = atoi(a);
    if (idx < 0 || idx >= NUM_FINGERS) { Serial.println(F("ERR idx")); return; }
    fingers[idx].setTarget(atof(b));
    Serial.println(F("OK"));

  } else if (!strcmp(cmd, "OPEN")) {
    setAll(0.0f);
    Serial.println(F("OK"));

  } else if (!strcmp(cmd, "GRASP")) {            // close until press limit
    setAll(100.0f);
    Serial.println(F("OK"));

  } else if (!strcmp(cmd, "LIM")) {              // LIM counts
    char *a = strtok_r(nullptr, " ", &save);
    if (!a) { Serial.println(F("ERR args")); return; }
    pressLimit = atof(a);
    Serial.println(F("OK"));

  } else if (!strcmp(cmd, "ZERO")) {
    for (uint8_t i = 0; i < NUM_FINGERS; i++) tactile[i].zero();
    Serial.println(F("OK"));

  } else if (!strcmp(cmd, "STREAM")) {           // STREAM 0|1
    char *a = strtok_r(nullptr, " ", &save);
    if (!a) { Serial.println(F("ERR args")); return; }
    streaming = atoi(a) != 0;
    Serial.println(F("OK"));

  } else if (!strcmp(cmd, "HELP")) {
    Serial.println(F("P p0..p4 | F i pct | OPEN | GRASP | LIM n | ZERO | STREAM 0/1"));
    Serial.println(F("OK"));

  } else {
    Serial.println(F("ERR cmd"));
  }
}

static void readSerial() {
  while (Serial.available()) {
    char c = static_cast<char>(Serial.read());
    if (c == '\r') continue;
    if (c == '\n') {
      rx[rxLen] = '\0';
      rxLen = 0;
      handleLine(rx);
      continue;
    }
    if (rxLen < RX_BUF_SIZE - 1) rx[rxLen++] = toupper(c);
    else rxLen = 0;                              // overflow: drop the line
  }
}

// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(SERIAL_BAUD);
  Wire.begin();

  pwm.begin();
  pwm.setOscillatorFrequency(PCA_OSC_HZ);
  pwm.setPWMFreq(PWM_FREQ_HZ);
  delay(10);

  for (uint8_t i = 0; i < NUM_FINGERS; i++) {
    fingers[i].begin(&pwm, SERVO_CH[i], PULSE_OPEN_US[i], PULSE_CLOSED_US[i],
                     MAX_SPEED_PCT_S);
    tactile[i].begin(HALL_PIN[i], HALL_SIGN, TACTILE_ALPHA, CONTACT_ON_COUNTS,
                     CONTACT_OFF_COUNTS);
  }

  Serial.println(F("# tendon-tactile-hand fw 0.2 (mega, hall)"));
  lastControl = lastStream = millis();
}

void loop() {
  readSerial();

  uint32_t now = millis();

  if (now - lastControl >= CONTROL_PERIOD_MS) {
    float dt = (now - lastControl) / 1000.0f;
    lastControl = now;
    for (uint8_t i = 0; i < NUM_FINGERS; i++) {
      tactile[i].update();
      fingers[i].update(dt, tactile[i].press() >= pressLimit);
    }
  }

  if (streaming && now - lastStream >= STREAM_PERIOD_MS) {
    lastStream = now;
    sendFrame(now);
  }
}
