#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MQUnifiedsensor.h>

#define board "Arduino Uno"
#define voltage_resolution 5 
#define pin A1
#define type "MQ-131"
#define ADC_Bit_Resolution 10
#define RatioMQ131CleanAir 15 

LiquidCrystal_I2C lcd(0x27, 16, 2);

MQUnifiedsensor MQ131(board, voltage_resolution, ADC_Bit_Resolution, pin, type);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Calibrating...");

  MQ131.init();
  
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ131.update(); 
    calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
    Serial.print(".");
  }
  MQ131.setR0(calcR0/10);
  Serial.println("  done!.");
 
  MQ131.serialDebug(false);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready!");
  delay(1000);
}

void loop() {
  MQ131.update(); 

  MQ131.setRegressionMethod(1); 
  MQ131.setA(23.943); MQ131.setB(-1.11); 
  
  float ppm = MQ131.readSensor();

  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("O3:");
  lcd.print(ppm); 
  lcd.print(" PPM");

  MQ131.readSensorR0Rs(); 
  delay(1000); 
}
