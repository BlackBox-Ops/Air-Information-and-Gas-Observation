#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>
#include <LowPower.h>

/*------------------------------------------
  Konstanta & Konfigurasi Pin
-------------------------------------------*/
#define DHTPIN 2
#define DHTTYPE DHT11
#define MQ_PIN A0
#define Board "Arduino UNO"
#define Voltage_Resolution 5
#define ADC_Bit_Resolution 10
#define RatioMQ6CleanAir 10
#define MQ_TYPE "MQ-6"

#define LedGreen  3
#define LedYellow 4
#define LedRed    5
#define BuzzerPin 6 // Pin buzzer aktif

/*------------------------------------------
  Inisialisasi Objek
-------------------------------------------*/
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ_PIN, MQ_TYPE);

/*------------------------------------------
  Ikon Custom LCD
-------------------------------------------*/
byte iconThermo[8] = {
  B00100, B01010, B01010, B01110,
  B01110, B11111, B11111, B01110
};

byte iconDrop[8] = {
  B00100, B00100, B01010, B01010,
  B10001, B10001, B10001, B01110
};

/*------------------------------------------
  Fungsi Membaca DHT11
-------------------------------------------*/
void bacaDHT(float &suhu, float &hum) {
  suhu = dht.readTemperature();
  hum = dht.readHumidity();
}

/*------------------------------------------
  Fungsi Reset Software
-------------------------------------------*/
void softReset() {
  asm volatile ("jmp 0");
}

/*------------------------------------------
  Setup Awal
-------------------------------------------*/
void setup() {
  pinMode(LedGreen, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(LedRed, OUTPUT);
  // pinMode(BuzzerPin, OUTPUT); // Buzzer
    
  lcd.init();
  lcd.backlight();
  dht.begin();

  MQ6.setRegressionMethod(1); // Linear mode
  MQ6.init();

  float calcR0 = 0;
  for (int i = 0; i < 10; i++) {
    MQ6.update();
    calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    delay(500);
  }
  MQ6.setR0(calcR0 / 10);
  MQ6.serialDebug(false);

  lcd.createChar(0, iconThermo);
  lcd.createChar(1, iconDrop);

  lcd.setCursor(0, 0);
  lcd.print("Membaca Sensor");
  delay(2000);
  lcd.clear();
}

/*------------------------------------------
  Loop Utama
-------------------------------------------*/
void loop() {
  unsigned long currentMillis = millis();
  static unsigned long prevReadTime = 0;
  const unsigned long intervalRead = 2000;

  // Pembacaan sensor setelah interval waktu tertentu
  if (currentMillis - prevReadTime >= intervalRead) {
    prevReadTime = currentMillis;

    MQ6.update();
    float suhu, hum;
    bacaDHT(suhu, hum);

    MQ6.setA(2127.2);
    MQ6.setB(-2.526);
    float ppm_gas1 = MQ6.readSensor();

    if (isnan(suhu) || isnan(hum)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor Error");
      delay(2000);
      softReset(); // reset kalau error
      return;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(0)); lcd.print(suhu, 1); lcd.print((char)223); lcd.print("C ");
    lcd.write(byte(1)); lcd.print(hum, 1); lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("CH4:");
    lcd.print(ppm_gas1, 1);

    // Matikan buzzer setelah sistem siap
    // digitalWrite(BuzzerPin,LOW);

    // Mode low power selama 1 detik setelah beberapa detik aktif
    delay(1000);  // Menunggu 1 detik sebelum memasuki mode tidur
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  // Tidur selama 8 detik untuk menghemat daya
  }
}
