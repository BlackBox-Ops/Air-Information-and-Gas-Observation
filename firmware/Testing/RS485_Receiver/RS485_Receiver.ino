#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DE_RE_PIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2); // Sesuaikan alamat I2C jika berbeda

void setup() {
  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW); // Tetap dalam mode penerima

  Serial.begin(9600); // RX digunakan untuk input dari MAX485
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menunggu...");
}

void loop() {
  static String inputString = "";
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Data: ");
      lcd.print(inputString);
      inputString = ""; // reset untuk pesan berikutnya
    } else {
      inputString += inChar;
    }
  }
}
