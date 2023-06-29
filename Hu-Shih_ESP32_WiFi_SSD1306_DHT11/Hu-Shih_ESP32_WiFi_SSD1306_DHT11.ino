/*
   Using ESP32 + SSD1306 + DHT11
   Detect Temp & Humi
   Data: 15
   SDA : 21
   SCL : 22
*/

#include <WiFi.h>
#include <DHT.h>
#include <Arduino.h>
#include <U8g2lib.h>

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define DHTPIN 15               //溫濕度接腳
#define DHTTYPE DHT11           //溫濕度感應器型號
DHT dht(DHTPIN, DHTTYPE);       //SET溫濕度感應器回傳

void setup() {
  Serial.begin(115200);
  dht.begin();
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
  u8g2.print("IP:");
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  delay(1000);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  delay(500);
  String OutputT = String("T:") + t +"°C";
  u8g2.setCursor(0, 12);
  u8g2.print(OutputT);
  String OutputH = String("H:") + h + "%";
  u8g2.setCursor(0, 26);
  u8g2.print(OutputH);
  u8g2.sendBuffer();
}
