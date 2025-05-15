#include <Arduino_FreeRTOS.h>
#include <MQUnifiedsensor.h>
#include <Servo.h>

// Konstanta sensor MQ-5
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0
#define type "MQ-5"
#define ADC_Bit_Resolution 10
#define RatioMQ5CleanAir 6.5


// Inisialisasi objek sensor dan servo
MQUnifiedsensor MQ5(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
Servo myServo;

// ---------- TASK BLINK LED 1 (PIN 2) ----------
void TaskBlink1(void *pvParameters) {
  pinMode(2, OUTPUT);
  for (;;) {
    digitalWrite(2, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    digitalWrite(2, LOW);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// ---------- TASK BLINK LED 2 (PIN 4) ----------
void TaskBlink2(void *pvParameters) {
  pinMode(4, OUTPUT);
  for (;;) {
    digitalWrite(4, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    digitalWrite(4, LOW);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// ---------- TASK BACA SENSOR GAS ----------
void TaskAnalogRead(void *pvParameters) {
  (void) pvParameters;

  MQ5.init();
  MQ5.setRegressionMethod(1); // Exponential
  MQ5.setR0(10); // Default sementara

  Serial.println("Kalibrasi MQ-5 sedang berlangsung...");
  float calcR0 = 0;
  for (int i = 0; i < 10; i++) {
    MQ5.update();
    calcR0 += MQ5.calibrate(RatioMQ5CleanAir);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  MQ5.setR0(calcR0 / 10);
  Serial.print("Kalibrasi selesai. R0 = ");
  Serial.println(MQ5.getR0());

  for (;;) {
    MQ5.update();

    // Baca H2
    MQ5.setA(1163.8); MQ5.setB(-3.875);
    float h2_ppm = MQ5.readSensor();

    // Baca LPG
    MQ5.setA(80.897); MQ5.setB(-2.431);
    float lpg_ppm = MQ5.readSensor();

    // Cetak hasil
    Serial.print("H2: ");
    Serial.print(h2_ppm);
    Serial.print(" ppm\t");

    Serial.print("LPG: ");
    Serial.print(lpg_ppm);
    Serial.println(" ppm");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// ---------- TASK GERAK SERVO ----------
void TaskServo(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    for (int angle = 0; angle <= 180; angle += 10) {
      myServo.write(angle);
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    for (int angle = 180; angle >= 0; angle -= 10) {
      myServo.write(angle);
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
  }
}

// ---------- TASK TOMBOL DAN LED MERAH ----------
void TaskButtonCheck(void *pvParameters) {
  pinMode(3, OUTPUT);
  pinMode(4, INPUT_PULLUP);  // Tombol aktif LOW

  for (;;) {
    if (digitalRead(4) == LOW) {
      digitalWrite(3, HIGH);  // Tombol ditekan
    } else {
      digitalWrite(3, LOW);   // Tombol dilepas
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Cek setiap 100ms
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(9600);
  myServo.attach(8);

  // Buat semua task
  xTaskCreate(TaskBlink1, "Blink1", 128, NULL, 1, NULL);
  xTaskCreate(TaskBlink2, "Blink2", 128, NULL, 1, NULL);
  xTaskCreate(TaskAnalogRead, "AnalogRead", 256, NULL, 2, NULL);
  xTaskCreate(TaskServo, "Servo", 128, NULL, 2, NULL);
  xTaskCreate(TaskButtonCheck, "Button", 128, NULL, 3, NULL);
}

// ---------- LOOP KOSONG KARENA RTOS ----------
void loop() {
  // FreeRTOS menjalankan semua task
}
