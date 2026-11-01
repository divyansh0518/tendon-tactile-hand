// tendon_hand.ino - serial-driven control of the tactile tendon hand.
// Keep the hand unloaded and open at power-up: the tactile baseline is zeroed
// during begin().
#include <stdlib.h>
#include <string.h>
#include "Config.h"
#include "Hand.h"

Hand hand;

static char lineBuf[48];
static uint8_t lineLen = 0;
static bool streaming = false;
static uint32_t lastControl = 0, lastTelemetry = 0;

static void printHelp() {
  Serial.println(F("# commands:"));
  Serial.println(F("#  open | grasp [level] | pose <name> | f <0-4> <0..1> | w <deg>"));
  Serial.println(F("#  level <counts> | cal | stream <0|1> | status | help"));
  Serial.print(F("#  poses:"));
  for (uint8_t k = 0; k < cfg::NUM_POSES; k++) { Serial.print(' '); Serial.print(cfg::POSES[k].name); }
  Serial.println();
}

static void printStatus() {
  Serial.print(F("# wrist=")); Serial.print(hand.wristDeg(), 1);
  Serial.print(F(" grasp_level=")); Serial.println(hand.graspLevel(), 1);
  for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) {
    Serial.print(F("# ")); Serial.print(cfg::FINGER_NAME[i]);
    Serial.print(F(" pos=")); Serial.print(hand.finger(i).position(), 3);
    Serial.print(F(" tactile=")); Serial.print(hand.sensor(i).level(), 1);
    Serial.print(F(" baseline=")); Serial.print(hand.sensor(i).baseline(), 1);
    Serial.print(F(" mode="));
    Serial.println(hand.mode(i) == Hand::FREE ? "free" : hand.mode(i) == Hand::CLOSING ? "closing" : "holding");
  }
}

static void printTelemetryHeader() {
  Serial.print(F("t_ms"));
  for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) { Serial.print(F(",pos_")); Serial.print(cfg::FINGER_NAME[i]); }
  Serial.print(F(",wrist_deg"));
  for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) { Serial.print(F(",tac_")); Serial.print(cfg::FINGER_NAME[i]); }
  for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) { Serial.print(F(",contact_")); Serial.print(cfg::FINGER_NAME[i]); }
  Serial.println();
}

static void printTelemetry() {
  Serial.print(millis());
  for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) { Serial.print(','); Serial.print(hand.finger(i).position(), 3); }
  Serial.print(','); Serial.print(hand.wristDeg(), 1);
  for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) { Serial.print(','); Serial.print(hand.sensor(i).level(), 1); }
  for (uint8_t i = 0; i < cfg::NUM_FINGERS; i++) { Serial.print(','); Serial.print(hand.sensor(i).contact() ? 1 : 0); }
  Serial.println();
}

static void handleCommand(char* s) {
  char* cmd = strtok(s, " ");
  if (!cmd) return;

  if (!strcmp(cmd, "help")) {
    printHelp();
  } else if (!strcmp(cmd, "open")) {
    hand.open();
  } else if (!strcmp(cmd, "grasp")) {
    char* a = strtok(NULL, " ");
    if (a) hand.setGraspLevel(atof(a));
    hand.grasp();
  } else if (!strcmp(cmd, "pose")) {
    char* a = strtok(NULL, " ");
    if (!a || !hand.pose(a)) Serial.println(F("# unknown pose, see help"));
  } else if (!strcmp(cmd, "f")) {
    char* a = strtok(NULL, " ");
    char* b = strtok(NULL, " ");
    if (!a || !b) { Serial.println(F("# usage: f <0-4> <0..1>")); return; }
    int idx = atoi(a);
    if (idx < 0 || idx >= cfg::NUM_FINGERS) { Serial.println(F("# finger index 0-4")); return; }
    hand.setFinger(idx, atof(b));
  } else if (!strcmp(cmd, "w")) {
    char* a = strtok(NULL, " ");
    if (a) hand.setWrist(atof(a)); else Serial.println(F("# usage: w <deg>"));
  } else if (!strcmp(cmd, "level")) {
    char* a = strtok(NULL, " ");
    if (a) hand.setGraspLevel(atof(a)); else Serial.println(F("# usage: level <counts>"));
  } else if (!strcmp(cmd, "cal")) {
    hand.recalibrateTactile();
    Serial.println(F("# tactile baseline re-zeroed"));
  } else if (!strcmp(cmd, "stream")) {
    char* a = strtok(NULL, " ");
    streaming = a && atoi(a) != 0;
    if (streaming) printTelemetryHeader();
  } else if (!strcmp(cmd, "status")) {
    printStatus();
  } else {
    Serial.println(F("# unknown command, try help"));
  }
}

static void pollSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (lineLen) { lineBuf[lineLen] = 0; handleCommand(lineBuf); lineLen = 0; }
    } else if (lineLen < sizeof(lineBuf) - 1) {
      lineBuf[lineLen++] = c;
    }
  }
}

void setup() {
  Serial.begin(cfg::BAUD);
  hand.begin();
  Serial.println(F("# tendon_hand ready, type 'help'"));
  lastControl = lastTelemetry = millis();
}

void loop() {
  pollSerial();

  uint32_t now = millis();
  if (now - lastControl >= cfg::CONTROL_PERIOD_MS) {
    float dt = (now - lastControl) * 0.001f;
    lastControl = now;
    hand.update(dt);
  }
  if (streaming && now - lastTelemetry >= cfg::TELEMETRY_PERIOD_MS) {
    lastTelemetry = now;
    printTelemetry();
  }
}
