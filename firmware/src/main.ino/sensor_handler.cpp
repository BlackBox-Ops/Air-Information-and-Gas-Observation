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

float SensorHandler::readGasVoltage() {
  return readGas() * (ADC_VOLTAGE_REF/ ADC_RESOLUTION);  
}

float SensorHandler::readRs() {
  float Vrl = readGasVoltage();   // Tegangan di pin A0
  float vcc = ADC_VOLTAGE_REF;    // 3.3v
  float RL  = 10.0;               // Ohm, resistor beban MQ2 (default 10K)
  return (vcc-Vrl)*RL/Vrl;        // Rumus : Rs = ((Vc - Vrl) * RL) / vrl
}

float SensorHandler::calculateRatio(float Ro) {
  return readRs() / Ro; 
}
