#include <MQUnifiedsensor.h>

#define Board "Arduino UNO"
#define Voltage_Resolution 5
#define Pin A0
#define Type "MQ-6"
#define ADC_Bit_Resolution 10
#define RatioMQ6CleanAir 10

MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

void setup() {
  Serial.begin(9600);

  MQ6.setRegressionMethod(1); // 1 = linear
  MQ6.setA(2127.2); 
  MQ6.setB(-2.526);

  MQ6.init();

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ6.update();
    calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    Serial.print("."); 
  }
  MQ6.setR0(calcR0 / 10);
  Serial.println("done!");

  MQ6.serialDebug(true);
}

void loop() {
  MQ6.update();      // Update data, the arduino will read the voltage from the analog pin
  MQ6.readSensor();  // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  MQ6.serialDebug(); // Will print the table on the serial port
  delay(500);        //Sampling frequency
}
