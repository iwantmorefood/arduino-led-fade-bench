#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

const uint8_t ledPin = 9;
const uint32_t minPeriodMs = 200;
const uint32_t maxPeriodMs = 60000;

char command[32];
size_t commandLength = 0;
bool lineTooLong = false;
bool fading = false;
uint8_t level = 0;
uint32_t periodMs = 2000;
uint32_t phaseMs = 0;
uint32_t lastTick = 0;

void setLevel(uint8_t nextLevel) {
  if (nextLevel != level) {
    level = nextLevel;
    analogWrite(ledPin, level);
  }
}

void printStatus() {
  Serial.print(F("mode="));
  Serial.print(fading ? F("fade") : F("fixed"));
  Serial.print(F(" level="));
  Serial.print(level);
  Serial.print(F(" period_ms="));
  Serial.println(periodMs);
}

bool readNumber(const char *text, uint32_t minimum, uint32_t maximum,
                uint32_t &value) {
  if (*text < '0' || *text > '9') {
    return false;
  }
  char *end;
  unsigned long number = strtoul(text, &end, 10);
  if (*end != '\0' || number < minimum || number > maximum) {
    return false;
  }
  value = number;
  return true;
}

void handleCommand() {
  uint32_t value;
  if (strcmp(command, "help") == 0) {
    Serial.println(F("Commands: level 0..255 | fade 200..60000 | stop | status | help"));
  } else if (strcmp(command, "status") == 0) {
    printStatus();
  } else if (strcmp(command, "stop") == 0) {
    fading = false;
    printStatus();
  } else if (strncmp(command, "level ", 6) == 0 &&
             readNumber(command + 6, 0, 255, value)) {
    fading = false;
    setLevel(static_cast<uint8_t>(value));
    printStatus();
  } else if (strncmp(command, "fade ", 5) == 0 &&
             readNumber(command + 5, minPeriodMs, maxPeriodMs, value)) {
    fading = true;
    periodMs = value;
    phaseMs = 0;
    lastTick = millis();
    setLevel(0);
    printStatus();
  } else {
    Serial.println(F("Invalid command or value. Send help for usage."));
  }
}

void readSerial() {
  // Cap work per loop so a burst of input does not monopolize updates.
  for (uint8_t count = 0; count < 32 && Serial.available() > 0; ++count) {
    const char input = static_cast<char>(Serial.read());
    if (input == '\r' || input == '\n') {
      if (lineTooLong) {
        Serial.println(F("Command too long."));
      } else if (commandLength > 0) {
        command[commandLength] = '\0';
        handleCommand();
      }
      lineTooLong = false;
      commandLength = 0;
    } else if (!lineTooLong) {
      if (commandLength == sizeof(command) - 1) {
        lineTooLong = true;
      } else {
        command[commandLength++] = input;
      }
    }
  }
}

void updateFade() {
  const uint32_t now = millis();
  const uint32_t elapsed = now - lastTick;
  lastTick = now;
  phaseMs = (phaseMs + elapsed % periodMs) % periodMs;

  const uint32_t peak = periodMs / 2;
  const uint32_t duty = phaseMs <= peak
    ? phaseMs * 255 / peak
    : (periodMs - phaseMs) * 255 / (periodMs - peak);
  setLevel(static_cast<uint8_t>(duty));
}

void setup() {
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);
  Serial.begin(115200);
  Serial.println(F("LED Fade Bench ready. Send help for commands."));
}

void loop() {
  readSerial();
  if (fading) {
    updateFade();
  }
}
