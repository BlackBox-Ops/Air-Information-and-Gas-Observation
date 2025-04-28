#include <AltSoftSerial.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

AltSoftSerial pmsSerial; // TX = 9, RX = 8 (default AltSoftSerial)
DHT dht(DHTPIN, DHTTYPE);

// Timer
unsigned long previousMillisDHT = 0;
unsigned long previousMillisPMS = 0;
const long intervalDHT = 2000;  // DHT dibaca tiap 2 detik
const long intervalPMS = 1000;  // PMS dicek tiap 1 detik

void setup() {
  Serial.begin(9600);
  pmsSerial.begin(9600);
  dht.begin();
  
  Serial.println("Starting...");
}

void loop() {
  unsigned long currentMillis = millis();

  // Baca DHT tiap 2 detik
  if (currentMillis - previousMillisDHT >= intervalDHT) {
    previousMillisDHT = currentMillis;
    bacaDHT();
  }

  // Cek data PMS tiap 1 detik
  if (currentMillis - previousMillisPMS >= intervalPMS) {
    previousMillisPMS = currentMillis;
    bacaPMS();
  }
}

// Fungsi baca DHT
void bacaDHT() {
  float suhu = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(suhu) && !isnan(hum)) {
    Serial.print("Temp: ");
    Serial.print(suhu);
    Serial.print(" *C, Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
  } else {
    Serial.println("DHT Error");
  }
}

// Fungsi baca PMS
void bacaPMS() {
  if (pmsSerial.available() >= 32) {
    if (pmsSerial.read() == 0x42) {
      if (pmsSerial.read() == 0x4D) {
        uint8_t buffer[32];
        buffer[0] = 0x42;
        buffer[1] = 0x4D;
        for (int i = 2; i < 32; i++) {
          buffer[i] = pmsSerial.read();
        }

        uint16_t pm1_0 = (buffer[4] << 8) | buffer[5];
        uint16_t pm2_5 = (buffer[6] << 8) | buffer[7];
        uint16_t pm10  = (buffer[8] << 8) | buffer[9];

        Serial.print("PM1.0: "); Serial.print(pm1_0);
        Serial.print(" ug/m3, PM2.5: "); Serial.print(pm2_5);
        Serial.print(" ug/m3, PM10: "); Serial.println(pm10);
      }
    }
  }
}
