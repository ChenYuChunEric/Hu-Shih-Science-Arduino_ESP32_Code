/*
   Using  ESP32 + SSD1306 + SGP30
   Detect TVOC & CO2
   SDA : 21
   SCL : 22
*/

#include <WiFi.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include "Adafruit_SGP30.h"

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Adafruit_SGP30 sgp;

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);  // Wait for serial console to open!
  }

  Serial.println(F("SGP30 test"));

  if (! sgp.begin()) {
    Serial.println(F("Sensor not found :("));
    while (1);
  }
  Serial.println(F("Found SGP30 serial"));

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

  // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
  //float temperature = 22.1; // [°C]
  //float humidity = 45.2; // [%RH]
  //sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp.IAQmeasure()) {
    Serial.println(F("Measurement failed"));
    return;
  }
  Serial.print(F("TVOC "));
  Serial.print(sgp.TVOC);
  Serial.println(F(" ppb"));
  Serial.print(F("eCO2 "));
  Serial.print(sgp.eCO2);
  Serial.println(F(" ppm"));

  if (! sgp.IAQmeasureRaw()) {
    Serial.println(F("Raw Measurement failed"));
    return;
  }
  delay(1000);

  u8g2.setCursor(0, 12);
  u8g2.print("TVOC ");
  u8g2.setCursor(0, 26);
  u8g2.print(sgp.TVOC);
  u8g2.print(" ppb");
  u8g2.setCursor(0, 44);
  u8g2.print("eCO2 ");
  u8g2.setCursor(0, 58);
  u8g2.print(sgp.eCO2);
  u8g2.print(" ppm");
  u8g2.sendBuffer();
}
