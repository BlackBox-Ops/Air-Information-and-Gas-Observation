#define DE_RE_PIN 2  // Pin kontrol DE dan RE

void setup() {
  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW); // Mulai dalam mode penerima

  Serial.begin(9600); // Inisialisasi komunikasi serial
}

void loop() {
  int analogValue = analogRead(A3); // Baca nilai analog dari A3

  // Aktifkan mode transmisi
  digitalWrite(DE_RE_PIN, HIGH);
  delayMicroseconds(10); // Delay singkat untuk memastikan mode aktif

  Serial.println(analogValue); // Kirim data

  Serial.flush(); // Tunggu hingga transmisi selesai

  // Kembali ke mode penerima
  digitalWrite(DE_RE_PIN, LOW);

  delay(1000); // Tunggu 1 detik sebelum pengiriman berikutnya
}
