#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MAX31865.h>

// setup pin MAX31865 (pakai SPI)
#define RREF      430.0  // Resistor referensi 430 ohm 
#define RNOMINAL  100.0  // PT100 nilai nominal 100 ohm 

// CS, D1, D0, CLK
Adafruit_MAX31865 rtd = Adafruit_MAX31865(10,11,12,13);

// LCD I2C 
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("RTD Sensor Init...");
  delay(2000);
  lcd.clear();

  // sesuaikan koneksi kabel : 2WIRE, 3WIRE, 4WIRE 
  rtd.begin(MAX31865_3WIRE); 
}

void loop() {
  uint16_t rtd_raw = rtd.readRTD();
  float resistance = rtd_raw;

  resistance /= 32768;
  resistance *= RREF;

  float temperature = rtd.temperature(RNOMINAL, RREF);

  // tampilan di serial monitor 
  Serial.print("RTD Resistance: "); Serial.println(resistance, 1);
  Serial.print("Temperature: "); Serial.println(temperature, 2);

  // Tampilan di LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C");

  
  lcd.setCursor(0, 1);
  lcd.print("Res:");
  lcd.print(resistance, 1);
  lcd.print(" Ohm");

  delay(2000);
}
