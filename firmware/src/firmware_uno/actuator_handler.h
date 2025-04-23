#ifndef ACTUATOR_HANDLER_H
#define ACTUATOR_HANDLER_H 

#define LEDPIN 4
#define BUZZERPIN 5

class ActuatorHandler {
  public:
    ActuatorHandler();
    void begin();
    void alert(bool state); 
    void blinkLED(int times, int delay_ms);
};

#endif
