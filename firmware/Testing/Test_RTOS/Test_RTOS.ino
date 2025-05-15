#include <Arduino_FreeRTOS.h>
#include <MQUnifiedsensor.h>
#include <Servo.h>

#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0
#define type "MQ-5"
#define ADC_Bit_Resolution 10 
#define RatioMQ5CleanAir 6.5
#define LED_MERAH_PIN 3
#define BUTTON_PIN 5

MQUnifiedsensor MQ5(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
Servo myServo;

void TaskBlink1(void *pvParameters) {
  pinMode(2, OUTPUT);
  for (;;) {
    digitalWrite(2, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS); 
    digitalWrite(2, LOW);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void TaskBlink2(void *pvParameters) {
  pinMode(4, OUTPUT);
  for (;;) {
    digitalWrite(4, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    digitalWrite(4, LOW);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void TaskAnalogRead(void *pvParameters) {
  (void) pvParameters;

  MQ5.init();
  MQ5.setRegressionMethod(1); // Exponential
  MQ5.setR0(10); // Default awal

  Serial.println("Calibrating MQ-5 for H2, please wait...");
  float calcR0 = 0;
  for (int i = 0; i < 10; i++) {
    MQ5.update();
    calcR0 += MQ5.calibrate(RatioMQ5CleanAir);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  MQ5.setR0(calcR0 / 10);
  Serial.print("Calibration completed. R0 = ");
  Serial.println(MQ5.getR0());

  for (;;) {
    MQ5.update();

    // --- Pembacaan H2 ---
    MQ5.setA(1163.8); MQ5.setB(-3.875);
    float h2_ppm = MQ5.readSensor();

    // --- Pembacaan LPG ---
    MQ5.setA(80.897); MQ5.setB(-2.431);
    float lpg_ppm = MQ5.readSensor();

    // --- Output Serial ---
    Serial.print("H2: ");
    Serial.print(h2_ppm);
    Serial.print(" ppm\t");

    Serial.print("LPG: ");
    Serial.print(lpg_ppm);
    Serial.println(" ppm");

    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Setiap 2 detik
  }
}

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

void setup() {
  Serial.begin(9600);
  myServo.attach(8);

  xTaskCreate(TaskBlink1, "Blink1", 128, NULL, 1, NULL);
  xTaskCreate(TaskBlink2, "Blink2", 128, NULL, 1, NULL);
  xTaskCreate(TaskAnalogRead, "AnalogRead", 256, NULL, 2, NULL);  // Stack size ditambah
  xTaskCreate(TaskServo, "Servo", 128, NULL, 1, NULL);
}

void loop() {
  // FreeRTOS handles tasks
}
