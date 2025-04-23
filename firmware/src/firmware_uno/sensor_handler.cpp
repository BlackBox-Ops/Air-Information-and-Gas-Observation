#include "sensor_handler.h"

SensorHandler::SensorHandler() : dht(DHTPIN, DHTTYPE) {}

void SensorHandler::begin() {
    pinMode(MQ2PIN, INPUT);
    dht.begin();
}

int SensorHandler::readRawGas() {
    return analogRead(MQ2PIN); // 0 - 1023 di Uno
}

float SensorHandler::readGasVoltage() {
    return readRawGas() * (ADC_VOLTAGE_REF / ADC_RESOLUTION);
}

float SensorHandler::readRs() {
    float Vrl = readGasVoltage();
    float RL = 10.0;
    return (ADC_VOLTAGE_REF - Vrl) * RL / Vrl;
}

float SensorHandler::calcRatio(float Ro) {
    return readRs() / Ro;
}

float SensorHandler::readTemperature() {
    return dht.readTemperature();
}

float SensorHandler::readHumidity() {
    return dht.readHumidity();
}
