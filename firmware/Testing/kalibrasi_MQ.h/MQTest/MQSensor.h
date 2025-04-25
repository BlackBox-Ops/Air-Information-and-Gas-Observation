#ifndef MQ6SENSOR_H
#define MQ6SENSOR_H

#include <Arduino.h>

class MQ6Sensor {
  private:
    int analogPin;
    float R0;  // kalibrasi
    float RL;  // resistor load

  public:
    MQ6Sensor(int pin, float r0 = 10.0, float rl = 5.0);
    int readADC();
    float readVoltage();
    float calculateRs();
    float readPPM(); // menggunakan model a*(Rs/R0)^b
};

#endif
