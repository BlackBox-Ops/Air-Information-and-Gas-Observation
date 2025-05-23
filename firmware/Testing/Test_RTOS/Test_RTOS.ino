#include <Arduino_FreeRTOS.h>
#include <MQUnifiedsensor.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleKalmanFilter.h>

// Sensor dan LCD
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0
#define type "MQ-5"
#define ADC_Bit_Resolution 10
#define RatioMQ5CleanAir 6.5

#define H2_THRESHOLD 4.36
#define LPG_THRESHOLD 5.10
#define LED_H2 2
#define LED_LPG 4

struct GasData {
  float h2_ppm;
  float lpg_ppm;
};

volatile GasData gasData; // volatile: agar akses antar task tetap valid

LiquidCrystal_I2C lcd(0x27, 16, 2);

SimpleKalmanFilter kalmanH2(2, 2, 0.01);   // (process noise, measurement noise, estimated error)
SimpleKalmanFilter kalmanLPG(2, 2, 0.01);

MQUnifiedsensor MQ5(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void TaskAnalogRead(void *pvParameters) {
  MQ5.setRegressionMethod(1); // Exponential
  MQ5.setA(1163.8); MQ5.setB(-3.875); // H2
  MQ5.init();
  MQ5.setR0(10);

  Serial.println("Kalibrasi MQ-5...");
  float calcR0 = 0;
  for (int i = 0; i < 10; i++) {
    MQ5.update();
    calcR0 += MQ5.calibrate(RatioMQ5CleanAir);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  MQ5.setR0(calcR0 / 10.0);
  Serial.print("R0: "); Serial.println(MQ5.getR0());

  for (;;) {
    MQ5.update();
    MQ5.setA(1163.8); MQ5.setB(-3.875);
    float h2_raw = MQ5.readSensor();
    gasData.h2_ppm = kalmanH2.updateEstimate(h2_raw);

    MQ5.setA(80.897); MQ5.setB(-2.431);
    float lpg_raw = MQ5.readSensor();
    gasData.lpg_ppm = kalmanLPG.updateEstimate(lpg_raw);

    Serial.print("H2: "); Serial.print(gasData.h2_ppm);
    Serial.print(" ppm\t");
    Serial.print("LPG: "); Serial.println(gasData.lpg_ppm);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void TaskLEDIndicator(void *pvParameters) {
  pinMode(LED_H2, OUTPUT);
  pinMode(LED_LPG, OUTPUT);

  for (;;) {
    digitalWrite(LED_H2, gasData.h2_ppm > H2_THRESHOLD ? LOW : HIGH);
    digitalWrite(LED_LPG, gasData.lpg_ppm > LPG_THRESHOLD ? LOW : HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void TaskLCDDisplay(void *pvParameters) {
  // PENTING: Inisialisasi hanya sekali di luar loop!
  lcd.init();
  lcd.backlight();
  lcd.clear();

  for (;;) {
    lcd.setCursor(0, 0);
    lcd.print("H2: ");
    lcd.print(gasData.h2_ppm, 2);
    lcd.print(" ppm   "); // clear sisa karakter

    lcd.setCursor(0, 1);
    lcd.print("LPG: ");
    lcd.print(gasData.lpg_ppm, 2);
    lcd.print(" ppm  ");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);

  // Hati-hati dengan alokasi stack untuk Uno (gunakan kecil)
  xTaskCreate(TaskAnalogRead, "Sensor", 192, NULL, 2, NULL);
  xTaskCreate(TaskLEDIndicator, "LED", 128, NULL, 1, NULL);
  xTaskCreate(TaskLCDDisplay, "LCD", 192, NULL, 1, NULL);
}

void loop() {
  // FreeRTOS loop kosong
}
