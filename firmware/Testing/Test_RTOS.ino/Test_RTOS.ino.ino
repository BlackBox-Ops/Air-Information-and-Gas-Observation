#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>

#define DHTPIN 2
#define DHTTYPE DHT11

#define Board "Arduino UNO"
#define Voltage_Resolution 5
#define Pin A0
#define Type "MQ-6"
#define ADC_Bit_Resolution 10
#define RatioMQ6CleanAir 10


MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ikon Termometer (index 0)
byte iconThermo[8] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

// Ikon Tetes Air (index 1)
byte iconDrop[8] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110
};

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();

  MQ6.setRegressionMethod(1); // 1 = linear
  MQ6.setA(2127.2); 
  MQ6.setB(-2.526);
  
  MQ6.init();
  for (int i = 1; i <= 10; i++) {
    MQ6.update();
    calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
  }
  MQ6.setR0(calcR0 / 10)
  MQ6.serialDebug(true);

  // Buat ikon khusus
  lcd.createChar(0, iconThermo);
  lcd.createChar(1, iconDrop);

  lcd.setCursor(0, 0);
  lcd.print("Membaca Sensor");
}

void loop() {

  float suhu = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(suhu) || isnan(hum)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    return;
  }

  lcd.clear();

  // Baris 0 saja
  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // ikon suhu
  lcd.print(suhu, 1);
  lcd.print((char)223); // derajat
  lcd.print("C ");

  lcd.write(byte(1));  // ikon kelembapan
  lcd.print(hum, 1);
  lcd.print("%");
}
