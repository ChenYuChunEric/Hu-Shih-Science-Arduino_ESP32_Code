/*
   Using  ESP32 + SSD1306 + MQ-131
   Detect O3 & Cl2
   Data: 34
   VCC : 3.3V only
   Note: May need Voltage divider rule
*/

#include <WiFi.h>
#include <MQUnifiedsensor.h>
#include <Arduino.h>
#include <U8g2lib.h>

//Definitions
#define placa "ESP32"
#define Voltage_Resolution 3.3
#define pin 34 //Analog input 0 of your arduino
#define type "MQ-131" //MQ135
#define ADC_Bit_Resolution 12
#define RatioMQ131CleanAir 15 //RS / R0 = 15 ppm 

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//Declare Sensor
MQUnifiedsensor MQ131(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

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
  
  MQ131.setRegressionMethod(1);
  MQ131.setA(23.94); MQ131.setB(-1.11);
  MQ131.setRL(1);
  MQ131.init();

  //-------校正--------
  Serial.print(F("Calibrating please wait."));
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
    Serial.print(F("."));
  }
  MQ131.setR0(calcR0 / 10);
  Serial.println();
  Serial.println(F("MQ-131  done!."));

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
  MQ131.setA(23.943); MQ131.setB(-1.11); // Configurate the ecuation values to get O3 concentration
  MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
  float O3ppb = MQ131.readSensorR0Rs(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

  delay(500);

  MQ131.setA(47.209); MQ131.setB(-1.186); // Configurate the ecuation values to get Cl2 concentration
  MQ131.update(); // Update data, the arduino will be read the voltage on the analog pin
  float Cl2ppm = MQ131.readSensorR0Rs(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

  delay(500);

  Serial.print(F("O3 = "));
  Serial.print(O3ppb);
  Serial.println(F(" ppb"));

  Serial.print(F("Cl2 = "));
  Serial.print(Cl2ppm);
  Serial.println(F(" ppm"));

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  u8g2.setCursor(0, 12);
  u8g2.print(F("O3 = "));
  u8g2.print(O3ppb);
  u8g2.println(F(" ppb"));
  u8g2.setCursor(0, 26);
  u8g2.print(F("Cl2 = "));
  u8g2.print(Cl2ppm);
  u8g2.println(F(" ppm"));
  u8g2.sendBuffer();
  
  delay(5000); //Sampling frequency

}
