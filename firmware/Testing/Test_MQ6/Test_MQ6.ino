#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MQUnifiedsensor.h>

// Setup Sensor MQ-6
#define Board "Arduino UNO"
#define Voltage_Resolution 5
#define Pin A0
#define Type "MQ-6"
#define ADC_Bit_Resolution 10
#define RatioMQ6CleanAir 10

// Setup LCD I2C (alamat bisa 0x27 atau 0x3F, tergantung modulmu)
LiquidCrystal_I2C lcd(0x27, 16, 2);

MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  Serial.print("Calibrating please wait.");

  MQ6.setRegressionMethod(1); // 1 -> Exponential
  MQ6.init();
  
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ6.update();
    calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    Serial.print(".");
    delay(500);
  }
  MQ6.setR0(calcR0 / 10);
  Serial.println(" done!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready!");
  delay(1000);
}

void loop() {
  MQ6.update();
  
  // --- Baca Gas 1 (misal: LPG) ---
  MQ6.setA(2127.2); 
  MQ6.setB(-2.526);
  float ppm_gas1 = MQ6.readSensor();

  // --- Baca Gas 2 (misal: Butane) ---
  MQ6.setA(1009.2); 
  MQ6.setB(-2.35);
  float ppm_gas2 = MQ6.readSensor();

  // --- Cetak ke Serial Monitor ---
  Serial.print("LPG: ");
  Serial.print(ppm_gas1);
  Serial.print(" ppm | ");
  Serial.print("Butane: ");
  Serial.println(ppm_gas2);

  // --- Tampilkan ke LCD ---
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("LPG:");
  lcd.print(ppm_gas1, 2); // 1 digit di belakang koma
  lcd.print(" ppm");

  lcd.setCursor(0, 1);
  lcd.print("Butane:");
  lcd.print(ppm_gas2, 1);
  lcd.print("ppm");

  delay(1000); // Update setiap 1 detik
}
