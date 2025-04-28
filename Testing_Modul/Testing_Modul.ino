#include <SoftwareSerial.h>

SoftwareSerial pmsSerial(8, 9); // RX di pin 8, TX di pin 9 (TX tidak dipakai)

uint8_t buffer[32];

void setup() {
  Serial.begin(9600);
  pmsSerial.begin(9600);
  delay(5000); // Tunggu PMS5003 ready
  Serial.println("Starting PMS5003 reading...");
}

void loop() {
  if (pmsSerial.available() >= 32) {
    if (pmsSerial.read() == 0x42) { // Harus 0x42
      if (pmsSerial.read() == 0x4D) { // lalu 0x4D
        buffer[0] = 0x42;
        buffer[1] = 0x4D;
        for (int i = 2; i < 32; i++) {
          buffer[i] = pmsSerial.read();
        }

        uint16_t pm1_0 = (buffer[4] << 8) | buffer[5];
        uint16_t pm2_5 = (buffer[6] << 8) | buffer[7];
        uint16_t pm10  = (buffer[8] << 8) | buffer[9];

        Serial.print("PM1.0 : ");
        Serial.print(pm1_0);
        Serial.print(" ug/m3 | ");

        Serial.print("PM2.5 : ");
        Serial.print(pm2_5);
        Serial.print(" ug/m3 | ");

        Serial.print("PM10 : ");
        Serial.println(pm10);
      }
    }
  }
}
