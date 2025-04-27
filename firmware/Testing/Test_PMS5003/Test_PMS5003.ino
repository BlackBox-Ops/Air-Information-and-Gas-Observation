#include "PMS.h"
#include "SoftwareSerial.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi Serial
SoftwareSerial Serial1(3, 2); // RX, TX

// Inisialisasi objek PMS dan LCD
PMS pms(Serial1);
PMS::DATA data;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 16x2 I2C dengan alamat 0x27

void setup()
{
  // Memulai komunikasi Serial
  Serial.begin(9600);
  Serial1.begin(9600);  // Memulai komunikasi dengan PMS

  // Memulai komunikasi LCD
  lcd.init();  // LCD 16x2
  lcd.backlight();   // Menghidupkan backlight LCD
  
  // Menampilkan pesan awal di LCD dan Serial Monitor
  lcd.setCursor(0, 0);
  lcd.print("Warming up...");
  Serial.println("Warming up...");
  delay(4000); // Menunggu sensor siap
  
  lcd.clear();
  Serial.println("Sensor siap!");
}

void loop()
{
  // Membaca data dari PMS
  if (pms.read(data))
  {
    // Menampilkan data di LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dust Concentration");

    // Menampilkan PM1.0 pada baris pertama LCD
    lcd.setCursor(0, 1);
    lcd.print("PM1.0: ");
    lcd.print(data.PM_AE_UG_1_0);
    lcd.print(" ug/m3");

    // Menampilkan data di Serial Monitor
    Serial.print("Dust Concentration\n");
    Serial.print("PM1.0: ");
    Serial.print(data.PM_AE_UG_1_0);
    Serial.println(" ug/m3");

    delay(1000); // Menunggu 1 detik sebelum update

    // Menampilkan PM2.5 pada baris pertama LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PM2.5: ");
    lcd.print(data.PM_AE_UG_2_5);
    lcd.print(" ug/m3");

    // Menampilkan data di Serial Monitor
    Serial.print("PM2.5: ");
    Serial.print(data.PM_AE_UG_2_5);
    Serial.println(" ug/m3");

    delay(1000);

    // Menampilkan PM10 pada baris pertama LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PM10: ");
    lcd.print(data.PM_AE_UG_10_0);
    lcd.print(" ug/m3");

    // Menampilkan data di Serial Monitor
    Serial.print("PM10: ");
    Serial.print(data.PM_AE_UG_10_0);
    Serial.println(" ug/m3");

    delay(1000); // Menunggu 1 detik sebelum update lagi
  }
}
