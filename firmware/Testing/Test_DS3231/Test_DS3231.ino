#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi RTC dan LCD
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Sesuaikan alamat jika perlu

// Karakter custom untuk ikon jam
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

void setup() {
  Wire.begin();
  lcd.init();
  lcd.createChar(0, clockIcon); // Buat ikon jam di slot 0
  lcd.backlight();
  
  if (!rtc.begin()) {
    lcd.print("RTC tidak ditemukan");
    while (1);
  }

  // Uncomment jika ingin set waktu (hanya sekali)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.write(byte(0)); // Tampilkan ikon jam
  // lcd.print(" Jam: ");

  // Format waktu
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());

  delay(1000);
}
