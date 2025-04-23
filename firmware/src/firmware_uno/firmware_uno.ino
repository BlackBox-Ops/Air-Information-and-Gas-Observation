#include "sensor_handler.h"
#include "actuator_handler.h"

SensorHandler sensor;
ActuatorHandler actuator;

float Ro = 10.0;

void setup() {
  Serial.begin(9600);
  sensor.begin();
  actuator.begin();
}

void loop() {
  
}
