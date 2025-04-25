#include "MQ6Sensor.h"
#include <math.h>

MQ6Sensor::MQ6Sensor(int pin, float r0, float rl) {
  analogPin = pin;
  R0 = r0;
  RL = rl;
}

int MQ6Sensor::readADC() {
  return analogRead(analogPin);
}

float MQ6Sensor::readVoltage() {
  return (readADC() / 1023.0) * 5.0;
}

float MQ6Sensor::calculateRs() {
  float Vout = readVoltage();
  if (Vout == 0) Vout = 0.01; // hindari div 0
  return ((5.0 - Vout) / Vout) * RL;
}

float MQ6Sensor::readPPM() {
  float Rs = calculateRs();
  float ratio = Rs / R0;

  // berdasarkan kurva datasheet, contoh: a = 1000, b = -1.5
  float a = 1000.0;
  float b = -1.5;

  return a * pow(ratio, b);
}
