#include "GasSensorSystem.h"

GasSensorSystem::GasSensorSystem() :
  MQ6(Board, Voltage_Resolution, ADC_BIT_RESOLUTION, MQ6_PIN, MQ6_TYPE),
  MQ131(Board, Voltage_Resolution, ADC_BIT_RESOLUTION, MQ131_PIN, MQ131_TYPE),
  lcd(0x27, 16, 2) {}

void GasSensorSystem::setup() {
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);
  Wire.begin();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  Serial.println("Calibrating please wait");

  if (!rtc.begin()) {
    lcd.print("RTC gagal");
    while (1);
  }

  MQ6.init(); MQ6.setRegressionMethod(1);
  MQ131.init(); MQ131.setRegressionMethod(1);
  calibrateSensors();

  Serial.println("Calibration selesai.");
  Serial.println("|  LPG  |  CH4 |  CL2  |  O3   |");

  digitalWrite(RelayPin, LOW); delay(500);
  digitalWrite(RelayPin, HIGH); delay(500);
}

void GasSensorSystem::calibrateSensors() {
  float MQ6calcR0 = 0, MQ131calcR0 = 0;
  for (int i = 0; i <= 10; i++) {
    MQ6.update(); MQ131.update();
    MQ6calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    MQ131calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
  }
  MQ6.setR0(MQ6calcR0 / 20);
  MQ131.setR0(MQ131calcR0 / 20);
}

void GasSensorSystem::readGases() {
  MQ6.update();
  MQ131.update();

  MQ6.setA(2127.2); MQ6.setB(-2.526); LPG = MQ6.readSensor();
  MQ6.setA(1009.2); MQ6.setB(-2.35);  CH4 = MQ6.readSensor();
  MQ131.setA(47.209); MQ131.setB(-1.186); CL2 = MQ131.readSensor();
  MQ131.setA(23.943); MQ131.setB(-1.11);  O3 = MQ131.readSensor();
}

void GasSensorSystem::displayReadings() {
  Serial.print("|  "); Serial.print(LPG);
  Serial.print(" | "); Serial.print(CH4);
  Serial.print(" |  "); Serial.print(CL2);
  Serial.print(" |  "); Serial.print(O3);
  Serial.println(" |");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CH4:"); lcd.print(CH4, 2);
  lcd.print("LPG:"); lcd.print(LPG, 2);

  lcd.setCursor(0, 1);
  lcd.print("CL2:"); lcd.print(CL2, 2);
  lcd.print(" O3:");  lcd.print(O3, 2);
}

void GasSensorSystem::update() {
  DateTime now = rtc.now();  // Time usage if needed
  float tempC = rtc.getTemperature();

  readGases();
  displayReadings();

  delay(1000); // Sampling rate
}
