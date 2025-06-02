/* Kode program Air Information and Gas Observation versi 4
 * Author : Alan Firdaus 
 */

#include <Arduino_FreeRTOS.h>    // Library untuk membuat sistem operasi realtime 
#include <MQUnifiedsensor.h>     // Library untuk kalibrasi tingkat lanjut untuk sensor MQ 
#include <LiquidCrystal_I2C.h>   // Library untuk interfacing tampilan LCD I2C
#include <SimpleKalmanFilter.h>  // Library untuk filter nilai sensor agar lebih bersih 
#include <Arduino.h>             // Library untuk menjalankan fungsi reset dari kode program 

//--------------- KONSTANTA SENSOR DAN DISPLAY LCD----------------
#define Board "Arduino Uno"     // Tipe Microcontroler Arduino Uno
#define Voltage_Resolution 5    // Tegangan Resolusi Sensor 5v
#define ADC_BIT 10              // Analog yang digunakan 10 Bit (0-1023)

//--------------- KONSTANTA UNTUK SENSOR MQ6----------------------
#define MQ6_PIN  A0             // Out Sensor MQ6 di pin AO
#define MQ6_TYPE "MQ6"          // Type Sensor MQ6
#define RatioMQ6CleanAir 10     // Parameter Udara Bersih 10

//--------------  KONSTANTA UNTUK SENSOR MQ131--------------------
#define MQ131_PIN  A1           // Out Sensor MQ131 di pin A1
#define MQ131_TYPE "MQ131"      // Type Sensor MQ131
#define RatioMQ131CleanAir 15   // Parameter Udara Bersih 15

//--------------  KONSTANTA UNTUK RELAY OUTPUT--------------------
#define RELAY_OUTPUT_LPG 2      // Relay output untuk paramater gas LPG di pin digital 2 arduino
#define RELAY_OUTPUT_CH4 3      // Relay output untuk parameter gas CH4 di pin digital 3 arduino
#define RELAY_OUTPUT_CL2 4      // Relay output untuk parameter gas CL2 di pin digital 4 arduino
#define RELAY_OUTPUT_O3  5      // Relay output untuk parameter gas O3 di pin digital 5 arduino

//--------------- KONSTANTA UNTUK SET POINT PARAMETER GAS----------
#define LPG_THRESHOLD 1.00      // Set Point Threshold untuk gas LPG
#define CH4_THRESHOLD 1.00      // Set Point Threshold untuk gas CH4
#define CL2_THRESHOLD 1.00      // Set Point Threshold untuk gas CL2
#define OZON_THRESHOLD  1.00    // Set Point Threshold untuk gas Ozon

//----------------Konstanta untuk reset otomatis by kode program 
#define RESET_INTERVAL_MS 3600000

// Buat variabel Gas Data untuk menyimpan data dari parameter gas 
struct GasData {
  float LPG_PPM;
  float CH4_PPM;
  float CL2_PPM;
  float O3_PPM;
};

//----------------Volatile agar antar task tetap valid-------------------- 
volatile GasData gasData;

//-----------INISIALISASI OBJECT SENSOR DAN MODUL EKSTERNAL---------------
MQUnifiedsensor MQ6(Board, Voltage_Resolution, ADC_BIT, MQ6_PIN, MQ6_TYPE);
MQUnifiedsensor MQ131(Board, Voltage_Resolution, ADC_BIT, MQ131_PIN, MQ131_TYPE);

// Parameter untuk object LCD 
LiquidCrystal_I2C lcd(0x27, 20, 4);

// konfigurasi parameter untuk filter nilai parameter gas 
SimpleKalmanFilter Filter_LPG(5, 1, 0.1); // Filter nilai parameter untuk raw gas LPG
SimpleKalmanFilter Filter_CH4(5, 1, 0.1); // Filter nilai parameter untuk raw gas CH4
SimpleKalmanFilter Filter_CL2(5, 1, 0.1); // Filter nilai parameter untuk raw gas CL2 
SimpleKalmanFilter Filter_O3(5, 1, 0.1);  // Filter nilai parameter untuk raw gas O3

// Buat Task untuk handle pembacaan sensor MQ 
void TaskMQSensor(void *pvParameters) {
  // Inisialisasi dan kalibrasi sensor gas tipe MQ6 dan MQ131
  MQ6.init(); MQ6.setRegressionMethod(1);
  MQ131.init(); MQ131.setRegressionMethod(1);

  float MQ6calcR0 = 0, MQ131calcR0 = 0;
  const int nCalib = 10;
  for (int i = 0; i < 10; i++) {
    MQ6.update(); MQ131.update();
    MQ6calcR0 += MQ6.calibrate(RatioMQ6CleanAir);
    MQ131calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  // Kalibrasi sensor MQ6 dan jug MQ131 
  MQ6.setR0(MQ6calcR0 / nCalib); 
  MQ131.setR0(MQ131calcR0 /nCalib);

  for(;;) {
    // baca sensor MQ 6 dan MQ 131 
    MQ6.update(); MQ131.update();

    /*
     * Kalibrasi untuk konsentrasi gas LPG dan Metana dari sensor MQ6
     * Kemudian filter raw data dari kosentrasi gas dengan menggunakan
     * kalman filter untuk mendapatkan hasil nilai sensor yang stabil 
     */
    MQ6.setA(2127.2); MQ6.setB(-2.526); float LPG_RAW = MQ6.readSensor(); gasData.LPG_PPM = Filter_LPG.updateEstimate(LPG_RAW);
    MQ6.setA(1009.2); MQ6.setB(-2.35);  float CH4_RAW = MQ6.readSensor(); gasData.CH4_PPM = Filter_CH4.updateEstimate(CH4_RAW);

    /*
     * Kalibrasi untuk kosentrasi gas Klorin dan Ozon
     * Kemudian filter raw data dari kosentrasi gas dengan menggunakan 
     * kalman filter untuk mendapatkan hasil nilai sensor yang stabil
     */
    MQ131.setA(47.209); MQ131.setB(-1.186); float CL2_RAW = MQ131.readSensor(); gasData.CL2_PPM = Filter_CL2.updateEstimate(CL2_RAW);
    MQ131.setB(23.943); MQ131.setB(-1.11);  float O3_RAW = MQ131.readSensor();  gasData.O3_PPM  = Filter_O3.updateEstimate(O3_RAW);

    // baca nilai sensor dari serial monitor 
    Serial.print("LPG : "); Serial.print(gasData.LPG_PPM);
    Serial.print(" PPM\t");
    Serial.print("CH4 : "); Serial.print(gasData.CH4_PPM);
    Serial.print("CL2 : "); Serial.print(gasData.CL2_PPM);
    Serial.print("O3 : ");  Serial.println(gasData.O3_PPM);
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Buat Task Untuk Handle Output Relay Berdasarkan Parameter Threshold parameter
void TaskRelayOutput(void *pvParameters) {
  // Relay Sebagai Output
  pinMode(RELAY_OUTPUT_LPG, OUTPUT);
  pinMode(RELAY_OUTPUT_CH4, OUTPUT);
  pinMode(RELAY_OUTPUT_CL2,OUTPUT);
  pinMode(RELAY_OUTPUT_O3, OUTPUT);

  // jika nilai paramenter > Threshold maka : 
  for(;;) {
    digitalWrite(RELAY_OUTPUT_LPG, gasData.LPG_PPM > LPG_THRESHOLD ? LOW : HIGH);
    digitalWrite(RELAY_OUTPUT_CH4, gasData.CH4_PPM > CH4_THRESHOLD ? LOW : HIGH);
    digitalWrite(RELAY_OUTPUT_CL2, gasData.CL2_PPM > CL2_THRESHOLD ? LOW : HIGH);
    digitalWrite(RELAY_OUTPUT_O3,  gasData.O3_PPM > OZON_THRESHOLD ? LOW : HIGH);

    // delay selama 500 detik
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Buat Task untuk Handle tampilan LCD 
void TaskLcdDisplay(void *pvParameters) {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  for(;;) {
    // Tampilan LCD Untuk Parameter Gas LPG 
    lcd.setCursor(0,0);
    lcd.print("LPG : ");
    lcd.print(gasData.LPG_PPM, 2);
    lcd.print("PPM");

    // Tampilan LCD Untuk Parameter Gas CH4
    lcd.setCursor(0,1);
    lcd.print("CH4 : ");
    lcd.print(gasData.CH4_PPM, 2);
    lcd.print("PPM");

    // Tampilan LCD Untuk Parameter Gas CL2
    lcd.setCursor(0,2);
    lcd.print("CL2 : ");
    lcd.print(gasData.CL2_PPM, 2);
    lcd.print("PPM");

    // Tampilan LCD Untuk Parameter Gas Ozon
    lcd.setCursor(0,3);
    lcd.print("03 : ");
    lcd.print(gasData.O3_PPM, 2);
    lcd.print("PPM");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Buat Task Untuk Reset by Kode Program 
void TaskSoftReset(void *pvParameters) {
  for(;;) {
    // Tunggu selama 1 jam kemudian reset otomatis 
    vTaskDelay(RESET_INTERVAL_MS / portTICK_PERIOD_MS);
    Serial.println("1 jam berlalu melakukan soft reset.....");

    // reset dengan function reset
    void(* resetFunc)(void) = 0;
    resetFunc();

    // Tidak pernah mencapai sini, hapus task jika sudah digunakan 
    vTaskDelete(NULL);
  }
}

void setup() {  
  // Buat Komunikasi Serial dengan Baud Rate (9600)
  Serial.begin(9600);

  // Buat Task RTOS 
  xTaskCreate(TaskMQSensor, "MQSensor", 192, NULL, 2, NULL);
  xTaskCreate(TaskRelayOutput,"LED", 128, NULL, 1, NULL);
  xTaskCreate(TaskLcdDisplay, "LCD", 128, NULL, 1, NULL);
  xTaskCreate(TaskSoftReset, "Soft Reset", 128, NULL, 1, NULL);
}

// Semua Fungsi di Jalankan FreeRTOS 
void loop() {}
