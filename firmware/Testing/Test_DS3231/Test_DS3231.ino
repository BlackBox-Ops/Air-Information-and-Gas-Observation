#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi RTC dan LCD
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Ganti 0x27 jika alamat I2C berbeda

// Ikon jam
byte clockIcon[8] = {
  0b00000,
  0b01110,
  0b10101,
  0b10111,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};

// Ikon termometer
byte thermoIcon[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b11111,
  0b11111,
  0b01110
};

void setup() {
  Wire.begin();
  lcd.init();
  lcd.createChar(0, clockIcon);  // Slot 0: ikon jam
  lcd.createChar(1, thermoIcon); // Slot 1: ikon termometer
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("RTC tidak ditemukan");
    while (1);
  }

  // Set waktu jika perlu
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();
  float tempC = rtc.getTemperature();

  // Baris 1: Jam
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); // Ikon jam
  lcd.print(" ");
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());

  // Baris 2: Suhu
  lcd.setCursor(0, 1);
  lcd.write(byte(1)); // Ikon termometer
  lcd.print(" ");
  lcd.print(tempC, 2); // Dua angka di belakang koma
  lcd.print((char)223); // Simbol derajat
  lcd.print("C");

  delay(1000);
}
