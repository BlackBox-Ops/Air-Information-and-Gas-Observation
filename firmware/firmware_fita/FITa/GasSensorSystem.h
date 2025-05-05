#ifndef GASSENSORSYSTEM_H
#define GASSENSORSYSTEM_H

#include <Wire.h>
#include <RTClib.h>
#include <MQUnifiedsensor.h>
#include <LiquidCrystal_I2C.h>

#define Board                 "Arduino Uno"
#define Voltage_Resolution    5
#define ADC_BIT_RESOLUTION    10
#define MQ6_PIN               A0
#define MQ131_PIN             A1
#define MQ6_TYPE              "MQ6"
#define MQ131_TYPE            "MQ131"
#define RatioMQ6CleanAir      10
#define RatioMQ131CleanAir    15
#define RelayPin              3

class GasSensorSystem {
  public:
    GasSensorSystem();
    void setup();
    void update();
  
  private:
    MQUnifiedsensor MQ6;
    MQUnifiedsensor MQ131;
    RTC_DS3231 rtc;
    LiquidCrystal_I2C lcd;

    float LPG, CH4, CL2, O3;
    void calibrateSensors();
    void displayReadings();
    void readGases();
};

#endif
