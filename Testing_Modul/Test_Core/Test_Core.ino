#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>

// === Konfigurasi Pin & Sensor ===
#define DHTPIN        2
#define DHTTYPE       DHT11
#define MQ6_PIN       A0
#define MQ131_PIN     A1
#define MQ6_HEATER    6   // ke gate MOSFET MQ6 (opsional)
#define MQ131_HEATER  7   // ke gate MOSFET MQ131 (opsional)

#define LED_GREEN     3
#define LED_YELLOW    4
#define LED_RED       5

#define Board         "Arduino UNO"
#define Voltage_Resolution 5
#define ADC_Bit_Resolution 10
#define RatioMQ6CleanAir   10
#define RatioMQ131CleanAir 15

// === Objek Sensor ===
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ6_PIN, "MQ-6");
MQUnifiedsensor MQ131(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ131_PIN, "MQ-131");

// === Variabel Global ===
unsigned long lastSwitchTime = 0;
const unsigned long sensorInterval = 5000;
enum SensorAktif { SENSOR_MQ6, SENSOR_MQ131 };
SensorAktif sensorAktif = SENSOR_MQ6;

float ppm_gas1 = 0;
float ppm_gas3 = 0;
float suhu = 0, hum = 0;

// === Ikon Kustom ===
byte iconThermo[8] = {
  B00100,B01010,B01010,B01110,
  B01110,B11111,B11111,B01110
};
byte iconDrop[8] = {
  B00100,B00100,B01010,B01010,
  B10001,B10001,B10001,B01110
};

// === Fungsi Modular ===
void setupSensorMQ(MQUnifiedsensor &sensor, float ratio) {
  sensor.setRegressionMethod(1);
  sensor.init();
  float r0 = 0;
  for (int i = 0; i < 10; i++) {
    sensor.update();
    r0 += sensor.calibrate(ratio);
    delay(300);
  }
  sensor.setR0(r0 / 10);
}

void bacaDHT() {
  suhu = dht.readTemperature();
  hum = dht.readHumidity();
}

void tampilkanDataLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); lcd.print(suhu, 1); lcd.print((char)223); lcd.print("C ");
  lcd.write(byte(1)); lcd.print(hum, 1); lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("CH4:");
  lcd.print(ppm_gas1, 1);
  lcd.print(" 03:");
  lcd.print(ppm_gas3, 1);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(MQ6_HEATER, OUTPUT);
  pinMode(MQ131_HEATER, OUTPUT);
  
  digitalWrite(MQ6_HEATER, HIGH);    // nyalakan MQ6 heater dulu
  digitalWrite(MQ131_HEATER, LOW);   // matikan MQ131 heater

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, iconThermo);
  lcd.createChar(1, iconDrop);
  lcd.setCursor(0, 0); lcd.print("Init Sensor...");
  dht.begin();

  setupSensorMQ(MQ6, RatioMQ6CleanAir);
  setupSensorMQ(MQ131, RatioMQ131CleanAir);

  lcd.clear();
  lcd.print("System Ready");
  delay(2000);
}

void loop() {
  bacaDHT();
  if (isnan(suhu) || isnan(hum)) return;

  unsigned long now = millis();
  if (now - lastSwitchTime >= sensorInterval) {
    lastSwitchTime = now;

    if (sensorAktif == SENSOR_MQ6) {
      // Aktifkan heater MQ6
      digitalWrite(MQ6_HEATER, HIGH);
      digitalWrite(MQ131_HEATER, LOW);

      MQ6.update();
      MQ6.setA(2127.2); MQ6.setB(-2.526); // LPG
      ppm_gas1 = MQ6.readSensor();
      sensorAktif = SENSOR_MQ131;

    } else {
      // Aktifkan heater MQ131
      digitalWrite(MQ6_HEATER, LOW);
      digitalWrite(MQ131_HEATER, HIGH);

      MQ131.update();
      MQ131.setA(23.943); MQ131.setB(-1.11); // O3
      ppm_gas3 = MQ131.readSensor();
      sensorAktif = SENSOR_MQ6;
    }
  }

  tampilkanDataLCD();
}
