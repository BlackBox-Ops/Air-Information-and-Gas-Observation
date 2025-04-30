#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>

// Pin dan konfigurasi sensor MQ 6 
#define DHTPIN 2
#define DHTTYPE DHT11
#define Board "Arduino UNO"
#define Voltage_Resolution 5
#define Pin A0
#define Type "MQ-6"
#define ADC_Bit_Resolution 10
#define RatioMQ6CleanAir 10

// bagian indikator 
#define LedGreen  3
#define LedYellow 4
#define LedRed    5

// Inisialisasi objek
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

// Ikon Termometer (index 0)
byte iconThermo[8] = {
  B00100,B01010,B01010,B01110,
  B01110,B11111,B11111,B01110
};

// Ikon Tetes Air (index 1)
byte iconDrop[8] = {
  B00100,B00100,B01010,B01010,
  B10001,B10001,B10001,B01110
};

// Fungsi membaca DHT
void bacaDHT(float &suhu, float &hum) {
  suhu = dht.readTemperature();
  hum = dht.readHumidity();
}

void setup() {
  pinMode(LedGreen,  OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(LedRed,OUTPUT);
  
  lcd.init();
  lcd.backlight();
  dht.begin();

  // Setup MQ6
  MQ6.setRegressionMethod(1); // 1 = linear
  MQ6.init();

  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ6.update();
    calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    delay(500);
  }
  MQ6.setR0(calcR0 / 10);
  MQ6.serialDebug(false); // matikan debug supaya hemat serial

  // Buat ikon khusus
  lcd.createChar(0, iconThermo);
  lcd.createChar(1, iconDrop);

  lcd.setCursor(0, 0);
  lcd.print("Membaca Sensor");
  delay(2000);
  lcd.clear();
}

void loop() {
  MQ6.update();
  
  float suhu, hum;
  bacaDHT(suhu, hum);
    
  // --- Baca Gas 1 (misal: LPG) ---
  MQ6.setA(2127.2); 
  MQ6.setB(-2.526);
  float ppm_gas1 = MQ6.readSensor();

  // --- Baca Gas 2 (misal: Butane) ---
  MQ6.setA(1009.2); 
  MQ6.setB(-2.35);
  float ppm_gas2 = MQ6.readSensor();

  if (isnan(suhu) || isnan(hum)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Up...");
    delay(2000);
    return;
  }

  lcd.clear();

  // Tampilkan suhu dan kelembapan di baris 0
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); // ikon suhu
  lcd.print(suhu, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.write(byte(1)); // ikon kelembapan
  lcd.print(hum, 1);
  lcd.print("%");

  // Tampilkan gas CH4 di baris 1
  lcd.setCursor(0, 1);
  lcd.print("CH4:");
  lcd.print(ppm_gas1, 1);
  // lcd.print(" H10:");
  // lcd.print(ppm_gas2, 1);
  

  delay(2000); // tunda supaya mudah dibaca
}
