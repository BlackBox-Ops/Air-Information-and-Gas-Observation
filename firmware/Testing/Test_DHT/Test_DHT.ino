#include <DHT.h>

#define DHTPIN 3
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Suhu: "); Serial.print(t); Serial.print(" C | ");
  Serial.print("Kelembapan: "); Serial.print(h); Serial.println(" %");
  delay(1000);
}
