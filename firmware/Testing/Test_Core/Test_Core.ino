#include <Wire.h>
#include <RTClib.h>
#include <MQUnifiedsensor.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define Board               "Arduino Uno"
#define Voltage_Resolution   5
#define MQ6_PIN              A0
#define MQ131_PIN            A1
#define MQ6_TYPE             "MQ6"
#define MQ131_TYPE           "MQ131"
#define ADC_BIT_RESOLUTION   10

#define RatioMQ6CleanAir     10
#define RatioMQ131CleanAir   15

#define RelayPin             3


#define DHTPIN               2
#define DHTTYPE              DHT21

DHT dht(DHTPIN, DHTTYPE); 

MQUnifiedsensor MQ6(Board,Voltage_Resolution, ADC_BIT_RESOLUTION, MQ6_PIN, MQ6_TYPE);
MQUnifiedsensor MQ131(Board,Voltage_Resolution, ADC_BIT_RESOLUTION, MQ131_PIN, MQ131_TYPE);

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastSwitch = 0;
bool showGasScreen = true;

void setup() {
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);

  Wire.begin();
  dht.begin();

  if (!rtc.begin()){
    lcd.print("RTC tidak ditemukan");
    while (1);
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Calibrating...");
  // Serial.print("Calibrating please wait");

  MQ6.init();
  MQ6.setRegressionMethod(1);
  
  MQ131.init();
  MQ131.setRegressionMethod(1);

  float MQ6calcR0 = 0, MQ131calcR0 = 0;
  for (int i = 0; i <= 10; i++) {
    MQ6.update();
    MQ131.update();  

    MQ6calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    MQ131calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
  }
  
  MQ6.setR0(MQ6calcR0/20);
  MQ131.setR0(MQ131calcR0/20);
  // Serial.println("done!.");

  // Serial.println("*********Values from MQ *********");
  // Serial.println("|  LPG  |  CH4 |  CL2  |  03   |");

  digitalWrite(RelayPin,LOW);
  delay(500);
  digitalWrite(RelayPin,HIGH);
  delay(500);
}

void loop() {
  DateTime now = rtc.now();
  MQ6.update(); MQ131.update();

  float dhtTemp = dht.readTemperature();
  float dhtHumi = dht.readHumidity();

  /*
   * Exponential regression
   * Sensor|Gas    |a     |b
   * MQ6   |LPG    |2127.2|-2.526
   * MQ6   |CH4    |1009.2|-2.35
   * MQ131 |CL2    |47.209|-1.186
   * MQ131 |O3     |23.943|-1.11
   */
   MQ6.setA(2127.2); MQ6.setB(-2.526); // Konsentrasi Gas LPG
   float LPG = MQ6.readSensor();

   MQ6.setA(1009.2); MQ6.setB(-2.35); // Konsentrasi Gas CH4
   float CH4 = MQ6.readSensor();

   MQ131.setA(47.209); MQ131.setB(-1.186); // Konsentrasi Gas CL2
   float CL2 = MQ131.readSensor();

   MQ131.setA(23.943); MQ131.setB(-1.11); // Konsentrasi Gas 03
   float O3 = MQ131.readSensor();

    // Bergantian tampilkan layar tiap 3 detik
  if (millis() - lastSwitch >= 3000) {
    showGasScreen = !showGasScreen;
    lastSwitch = millis();
  }

  lcd.clear();
  if (showGasScreen) 
  {
    lcd.setCursor(0, 0);
    lcd.print("CH4:"); lcd.print(CH4, 2);
    lcd.print("LPG:"); lcd.print(LPG, 2);

    lcd.setCursor(0, 1);
    if (now.hour() < 10)     lcd.print('0');
    lcd.print(now.hour());   lcd.print(':');
    if (now.minute() < 10)   lcd.print('0');
    lcd.print(now.minute()); lcd.print(':');
    if (now.second() < 10)   lcd.print('0');
    lcd.print(now.second());
  } 
  else {
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(dhtTemp, 1); lcd.print((char)223); lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("H:"); lcd.print(dhtHumi, 1); lcd.print("%");
  }

  delay(3000);
}
