#include "sensor_handler.h"

SensorHandler sensor;
float Ro = 10.0; // Ro = Rs di udara bersih (kalibrasi manual)

void setup() {
    Serial.begin(9600);
    sensor.begin();
}

void loop() {
    float temp = sensor.readTemperature();
    float hum = sensor.readHumidity();
    float rs = sensor.readRs();
    float ratio = sensor.calculateRatio(Ro);

    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" °C | Hum: ");
    Serial.print(hum);
    Serial.print(" % | Gas: ");
    Serial.println(ratio);

    Serial.print("Gas Voltage: ");
    Serial.print(sensor.readGasVoltage());
    Serial.print(" V | Rs: ");
    Serial.print(rs);
    Serial.print(" kOhm | Rs/Ro: ");
    Serial.print(ratio);

    if (ratio < 0.5) Serial.println(" => Alkohol");
    else if (ratio < 1.2) Serial.println(" => LPG");
    else if (ratio < 2.0) Serial.println(" => Methane/Propane");
    else if (ratio < 3.5) Serial.println(" => Smoke");
    else Serial.println(" => CO/Unknown");

    delay(2000);
}
