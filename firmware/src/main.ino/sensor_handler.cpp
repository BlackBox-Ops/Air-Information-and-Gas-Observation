#include "sensor_handler.h"

SensorHandler::SensorHandler() : dht(DHTPIN, DHTTYPE) {}

void SensorHandler::begin() {
    dht.begin();
    pinMode(MQ2PIN, INPUT);
}

float SensorHandler::readTemperature() {
    return dht.readTemperature();
}

float SensorHandler::readHumidity() {
    return dht.readHumidity();
}

int SensorHandler::readGas() {
    return analogRead(MQ2PIN);
}
