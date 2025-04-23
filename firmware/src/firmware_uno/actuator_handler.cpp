#include "actuator_handler.h"
#include <Arduino.h>

ActuatorHandler::ActuatorHandler() {}

void ActuatorHandler::begin() {
    pinMode(LEDPIN, OUTPUT);
    pinMode(BUZZERPIN, OUTPUT);
}

void ActuatorHandler::alert(bool state) {
    digitalWrite(LEDPIN, state);
    digitalWrite(BUZZERPIN, state);
}

void ActuatorHandler::blinkLED(int times, int delay_ms) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LEDPIN, HIGH);
        delay(delay_ms);
        digitalWrite(LEDPIN, LOW);
        delay(delay_ms);
    }
}
