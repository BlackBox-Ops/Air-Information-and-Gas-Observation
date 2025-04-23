#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define MQ2PIN A0

class SensorHandler {
public:
    SensorHandler();
    void begin();
    float readTemperature();
    float readHumidity();
    int readGas();

private:
    DHT dht;
};

#endif
