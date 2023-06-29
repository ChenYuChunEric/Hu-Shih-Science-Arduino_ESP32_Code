/*
   Using  ESP32 + SSD1306 + MQ-7
   Detect CO
   Data: 36
   VCC : 3.3V only
*/

#include <WiFi.h>
#include <MQUnifiedsensor.h>
#include <Arduino.h>
#include <U8g2lib.h>

//Definitions
#define placa "ESP32"
#define Voltage_Resolution 5
#define pin 36 //Analog input 0 of your arduino
#define type "MQ-7" //MQ7
#define ADC_Bit_Resolution 12
#define RatioMQ7CleanAir 27.5 //RS / R0 = 27.5 ppm 

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//Declare Sensor
MQUnifiedsensor MQ7(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

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
  
  MQ7.setRegressionMethod(1);
  MQ7.setA(99.042); MQ7.setB(-1.518);
  MQ7.init();

  //-------校正--------
  Serial.print(F("Calibrating please wait."));
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
    Serial.print(F("."));
  }
  MQ7.setR0(calcR0 / 10);
  Serial.println();
  Serial.println(F("MQ-7  done!."));

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
  MQ7.update(); // Update data, the arduino will be read the voltage on the analog pin
  float COppm = MQ7.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
  Serial.print(F("CO= "));
  Serial.print(COppm);
  Serial.println(F(" ppm"));

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  u8g2.setCursor(0, 12);
  u8g2.print(F("CO: "));
  u8g2.print(COppm);
  u8g2.println(F(" ppm"));
  u8g2.sendBuffer();
  
  delay(5000); //Sampling frequency

}
