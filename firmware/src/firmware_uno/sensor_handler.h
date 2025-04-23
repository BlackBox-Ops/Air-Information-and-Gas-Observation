#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <DHT.h>

#define MQ2PIN A0
#define DHTPIN 2
#define DHTTYPE DHT11
#define ADC_RESOLUTION 1023.0
#define ADC_VOLTAGE_REF 5.0

class SensorHandler {
public:
    SensorHandler();
    void begin();
    int readRawGas();
    float readGasVoltage();
    float readRs();
    float calcRatio(float Ro);
    float readTemperature();
    float readHumidity();

private:
    DHT dht;
};

#endif
