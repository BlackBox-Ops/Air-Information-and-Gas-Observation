#include "MQ6Sensor.h"

MQ6Sensor mq6(A0, 10.0, 5.0);  // pin A0, R0 = 10kΩ, RL = 5kΩ

void setup() {
  Serial.begin(9600);
}

void loop() {
  int adc = mq6.readADC();
  float volt = mq6.readVoltage();
  float ppm = mq6.readPPM();

  Serial.print("ADC: ");
  Serial.print(adc);
  Serial.print(" | Vout: ");
  Serial.print(volt, 2);
  Serial.print(" V | Estimasi PPM: ");
  Serial.println(ppm, 2);

  delay(1000);
}
