#include "sensor_handler.h"

SensorHandler sensor;

void setup() {
    Serial.begin(9600);
    sensor.begin();
}

void loop() {
    float temp = sensor.readTemperature();
    float hum = sensor.readHumidity();
    int gas = sensor.readGas();

    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" °C | Hum: ");
    Serial.print(hum);
    Serial.print(" % | Gas: ");
    Serial.println(gas);

    delay(2000);
}
