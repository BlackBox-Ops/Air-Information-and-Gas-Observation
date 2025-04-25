#include "MQSensor.h"

MQSensor mq(A0, 10.0);

void setup() {
  Serial.begin(9600);
  mq.begin();
}

void loop() {
  float ppm = mq.readPPM();
  Serial.print("Gas LPG [ppm] : ");
  Serial.println(ppm);
  delay(2000);
}
