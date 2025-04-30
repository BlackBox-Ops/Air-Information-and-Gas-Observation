#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>

// === Konstanta ===
#define DHTPIN 2
#define DHTTYPE DHT11

#define BOARD "Arduino UNO"
#define VOLTAGE_RESOLUTION 5
#define ADC_RESOLUTION 10

#define PIN_MQ6 A0
#define PIN_MQ131 A1

#define TYPE_MQ6 "MQ-6"
#define TYPE_MQ131 "MQ-131"

#define RATIO_MQ6_CLEAN_AIR   10
#define RATIO_MQ131_CLEAN_AIR 15

#define LED_GREEN  3
#define LED_YELLOW 4
#define LED_RED    5

#define INTERVAL_SENSOR 2000  // interval pembacaan sensor

// === Objek ===
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MQUnifiedsensor MQ6(BOARD, VOLTAGE_RESOLUTION, ADC_RESOLUTION, PIN_MQ6, TYPE_MQ6);
MQUnifiedsensor MQ131(BOARD, VOLTAGE_RESOLUTION, ADC_RESOLUTION, PIN_MQ131, TYPE_MQ131);

// === Variabel Global ===
unsigned long lastReadTime = 0;
float suhu = 0, hum = 0;
float ppm_ch4 = 0, ppm_o3 = 0;

// === Ikon LCD ===
byte iconThermo[8] = {
  B00100, B01010, B01010, B01110,
  B01110, B11111, B11111, B01110
};
byte iconDrop[8] = {
  B00100, B00100, B01010, B01010,
  B10001, B10001, B10001, B01110
};

// === Fungsi Reusable ===
void tampilkanIkon() {
  lcd.createChar(0, iconThermo);
  lcd.createChar(1, iconDrop);
}

float kalibrasiSensor(MQUnifiedsensor &sensor, float cleanAirRatio) {
  float r0 = 0;
  for (int i = 0; i < 10; i++) {
    sensor.update();
    r0 += sensor.calibrate(cleanAirRatio);
    delay(100); // Tetap blocking saat setup
  }
  return r0 / 10.0;
}

void bacaSemuaSensor() {
  // Baca DHT
  suhu = dht.readTemperature();
  hum = dht.readHumidity();

  // Baca CH4 (MQ6)
  MQ6.setA(2127.2);
  MQ6.setB(-2.526);
  MQ6.update();
  ppm_ch4 = MQ6.readSensor();

  // Baca O3 (MQ131)
  MQ131.setA(23.943);
  MQ131.setB(-1.11);
  MQ131.update();
  ppm_o3 = MQ131.readSensor();
}

void tampilkanLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); lcd.print(suhu, 1); lcd.print((char)223); lcd.print(F("C "));
  lcd.write(byte(1)); lcd.print(hum, 1); lcd.print(F("%"));

  lcd.setCursor(0, 1);
  lcd.print(F("CH4:")); lcd.print(ppm_ch4, 1);
  lcd.print(F(" 03:")); lcd.print(ppm_o3, 1);
}

// === Setup ===
void setup() {
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  lcd.init();
  lcd.backlight();
  tampilkanIkon();

  dht.begin();

  MQ6.setRegressionMethod(1);
  MQ6.init();
  MQ6.setR0(kalibrasiSensor(MQ6, RATIO_MQ6_CLEAN_AIR));

  MQ131.setRegressionMethod(1);
  MQ131.init();
  MQ131.setR0(kalibrasiSensor(MQ131, RATIO_MQ131_CLEAN_AIR));

  lcd.setCursor(0, 0);
  lcd.print(F("Membaca Sensor"));
  delay(1500);
  lcd.clear();
}

// === Loop Non-Blocking ===
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastReadTime >= INTERVAL_SENSOR) {
    lastReadTime = currentMillis;

    bacaSemuaSensor();

    if (isnan(suhu) || isnan(hum)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Error DHT11..."));
      return;
    }

    tampilkanLCD();
  }

  // Tambahkan logika LED jika perlu
  // contoh:
  // digitalWrite(LED_GREEN, ppm_ch4 < 100 ? HIGH : LOW);
}
