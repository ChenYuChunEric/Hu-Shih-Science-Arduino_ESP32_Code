/*
   Using  ESP32 + SSD1306 + PMS5003T
   Detect PM1.0 & 2.5 & 10
   PMS_RX: 35
   PMS_TX: 32
*/

#include <WiFi.h>
#include <PMserial.h>
#include <Arduino.h>
#include <U8g2lib.h>

#define PMS_RX 35
#define PMS_TX 32

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

SerialPM pms(PMS5003, PMS_RX, PMS_TX); // PMSx003, RX, TX


void setup() {
  Serial.begin(115200);
  pms.init();

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
  pms.read();
  if (pms)
  {
    Serial.print(F("PM1.0 "));
    Serial.print(pms.pm01);
    Serial.print(F(", "));
    Serial.print(F("PM2.5 "));
    Serial.print(pms.pm25);
    Serial.print(F(", "));
    Serial.print(F("PM10 "));
    Serial.print(pms.pm10);
    Serial.println(F(" [ug/m3]"));
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  u8g2.setCursor(0, 12);
  u8g2.print("PM1.0: ");
  u8g2.print(pms.pm01);
  u8g2.setCursor(0, 26);
  u8g2.print("PM2.5: ");
  u8g2.print(pms.pm25);
  u8g2.setCursor(0, 40);
  u8g2.print("PM10 : ");
  u8g2.print(pms.pm10);
  u8g2.sendBuffer();

  delay(5000);
}
