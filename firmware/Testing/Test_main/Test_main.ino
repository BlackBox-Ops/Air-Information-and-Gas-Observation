#include <Wire.h>
#include <RTClib.h>
#include <MQUnifiedsensor.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

/**
 * Konstanta dan definisi pin sensor
 */
#define Board               "Arduino Uno"
#define Voltage_Resolution  5
#define ADC_Bit             10

#define MQ6_PIN             A0
#define MQ131_PIN           A1
#define MQ6_TYPE            "MQ6"
#define MQ131_TYPE          "MQ131"
#define RatioMQ6CleanAir    10
#define RatioMQ131CleanAir  15

#define RelayPin            3
#define DHTPIN              2
#define DHTTYPE             DHT21

/**
 * Inisialisasi objek sensor dan modul eksternal
 */
DHT dht(DHTPIN, DHTTYPE);
MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit, MQ6_PIN, MQ6_TYPE);
MQUnifiedsensor MQ131(Board, Voltage_Resolution, ADC_Bit, MQ131_PIN, MQ131_TYPE);
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

/**
 * Variabel global: untuk penjadwalan tampilan LCD bergantian
 */
unsigned long lastSwitch = 0;
bool showGasScreen = true;

/**
 * Fungsi setup() berjalan satu kali saat board menyala
 * - Inisialisasi sensor
 * - Kalibrasi MQ6 & MQ131
 * - Setup RTC & LCD
 */
void setup() {
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);

  Wire.begin();
  dht.begin();
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("RTC tidak ditemukan");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");

  // Inisialisasi dan kalibrasi sensor gas
  MQ6.init(); MQ6.setRegressionMethod(1);
  MQ131.init(); MQ131.setRegressionMethod(1);

  float MQ6calcR0 = 0, MQ131calcR0 = 0;
  const int nCalib = 11;

  for (int i = 0; i < nCalib; i++) {
    MQ6.update(); MQ131.update();
    MQ6calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    MQ131calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
    delay(100);
  }

  MQ6.setR0(MQ6calcR0 / nCalib);
  MQ131.setR0(MQ131calcR0 / nCalib);

  // Trigger relay sesaat (jika diperlukan)
  digitalWrite(RelayPin, LOW);
  delay(300);
  digitalWrite(RelayPin, HIGH);
}

/**
 * Fungsi loop() berjalan terus-menerus
 * - Membaca data dari sensor gas dan DHT
 * - Menampilkan data ke LCD secara bergantian
 */
void loop() {
  DateTime now = rtc.now();

  MQ6.update(); MQ131.update();
  float dhtTemp = dht.readTemperature();
  float dhtHumi = dht.readHumidity();

  // Baca dan hitung konsentrasi gas dari MQ6
  MQ6.setA(2127.2); MQ6.setB(-2.526); float LPG = MQ6.readSensor();
  MQ6.setA(1009.2); MQ6.setB(-2.35);  float CH4 = MQ6.readSensor();

  // Baca dan hitung konsentrasi gas dari MQ131
  MQ131.setA(47.209); MQ131.setB(-1.186); float CL2 = MQ131.readSensor();
  MQ131.setA(23.943); MQ131.setB(-1.11);  float O3 = MQ131.readSensor();

  // Tampilkan data bergantian setiap 3 detik
  if (millis() - lastSwitch >= 3000) {
    showGasScreen = !showGasScreen;
    lastSwitch = millis();
    lcd.clear();
  }

  if (showGasScreen) {
    /**
     * Tampilan gas: CH4 & LPG + waktu dari RTC
     */
    lcd.setCursor(0, 0);
    lcd.print("CH4:"); lcd.print(CH4, 2);
    lcd.print(" LPG:"); lcd.print(LPG, 2);

    lcd.setCursor(0, 1);
    lcd.print(now.hour()   < 10 ? "0" : ""); lcd.print(now.hour()); lcd.print(":");
    lcd.print(now.minute() < 10 ? "0" : ""); lcd.print(now.minute()); lcd.print(":");
    lcd.print(now.second() < 10 ? "0" : ""); lcd.print(now.second());
  } else {
    /**
     * Tampilan lingkungan: suhu & kelembaban
     */
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(dhtTemp, 1); lcd.print((char)223); lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("H:"); lcd.print(dhtHumi, 1); lcd.print("%");
  }

  delay(500); // jeda kecil agar sistem tetap responsif
}
