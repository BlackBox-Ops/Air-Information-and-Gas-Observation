#include <Arduino_FreeRTOS.h>
#include "sensor_handler.h"
#include "actuator_handler.h"

// Buat instance dari class sensor dan actuator
SensorHandler sensor;
ActuatorHandler actuator;

// Variabel global (bisa volatile kalau perlu)
float gasRatio = 0;
float temp = 0;
float hum = 0;
float Ro = 10.0;

// ======== DEFINISI TASK LANGSUNG DI .ino ========

void TaskSensorRead(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    gasRatio = sensor.calcRatio(Ro);
    temp = sensor.readTemperature();
    hum = sensor.readHumidity();

    Serial.print("[Sensor] GasRatio: ");
    Serial.print(gasRatio);
    Serial.print(" | Temp: ");
    Serial.print(temp);
    Serial.print(" C | Hum: ");
    Serial.println(hum);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void TaskControlActuator(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    bool alertCondition = false;

    if (gasRatio < 0.5 || gasRatio < 1.2 || temp > 40 || hum > 90) {
      alertCondition = true;
    }

    actuator.alert(alertCondition);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// ======== SETUP DAN LOOP ========

void setup() {
  Serial.begin(9600);
  sensor.begin();
  actuator.begin();

  xTaskCreate(TaskSensorRead, "ReadSensor", 128, NULL, 1, NULL);
  xTaskCreate(TaskControlActuator, "Actuate", 128, NULL, 1, NULL);
}

void loop() {
  // Tidak dipakai saat FreeRTOS berjalan
}
