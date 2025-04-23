#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define MQ2PIN A0
#define ADC_RESOLUTION 4095.0 
#define ADC_VOLTAGE_REF 3.3 

class SensorHandler {
public:
    SensorHandler();
    void begin();
    float readTemperature();
    float readHumidity();
    int readGas();
    float readGasVoltage();
    float readRs();
    float calculateRatio(float Ro);

private:
    DHT dht;
};

#endif
