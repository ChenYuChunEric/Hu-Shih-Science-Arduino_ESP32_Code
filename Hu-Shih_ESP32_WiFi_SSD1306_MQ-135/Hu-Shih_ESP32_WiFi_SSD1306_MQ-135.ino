/*
   Using  ESP32 + SSD1306 + MQ-135
   Detect NH4
   Data: 39
   VCC : 3.3V only
   Note: AO need Voltage divider rule
*/

#include <WiFi.h>
#include <MQUnifiedsensor.h>
#include <Arduino.h>
#include <U8g2lib.h>

//Definitions
#define placa "ESP32"
#define Voltage_Resolution 3.3
#define pin 39 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 12
#define RatioMQ135CleanAir 3.6 //RS / R0 = 27.5 ppm 

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 16, "Connecting to WiFi...");
  u8g2.sendBuffer();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  u8g2.drawStr(0, 32, "Connected.");
  u8g2.setCursor(0, 48);
  u8g2.print(F("IP:"));
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  delay(1000);
  
  MQ135.setRegressionMethod(1);
  MQ135.setA(102.2); MQ135.setB(-2.473);
  MQ135.setRL(1);
  MQ135.init();

  //-------校正--------
  Serial.print(F("Calibrating please wait."));
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(F("."));
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println();
  Serial.println(F("MQ-135  done!."));

  if (isinf(calcR0)) {
    Serial.println(F("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"));
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println(F("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"));
    while (1);
  }
  //-------校正--------
}

void loop() {
  MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
  float NH4ppm = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  Serial.print(F("NH4= "));
  Serial.print(NH4ppm);
  Serial.println(F("ppm"));

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  u8g2.setCursor(0, 12);
  u8g2.print(F("NH4= "));
  u8g2.print(NH4ppm);
  u8g2.println(F(" ppm"));
  u8g2.sendBuffer();
  
  delay(5000); //Sampling frequency

}
