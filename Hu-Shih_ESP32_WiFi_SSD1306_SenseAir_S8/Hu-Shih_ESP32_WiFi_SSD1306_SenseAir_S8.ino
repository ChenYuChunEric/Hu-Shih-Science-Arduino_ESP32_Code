/*
   Using  ESP32 + SSD1306 + SenseAir S8
   Detect CO2
   RX2: 16
   TX2: 17
*/

#include <WiFi.h>
#include <U8g2lib.h>
#include <Arduino.h>
#include "s8_uart.h"

#define S8_UART_PORT  2     // RX2: 16  TX2: 17

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

HardwareSerial S8_serial(S8_UART_PORT);
S8_UART *sensor_S8;
S8_sensor sensor;

void setup() {

  // Configure serial port, we need it for debug
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();

  // First message, we are alive
  Serial.println(F("Init S8"));

  // Initialize S8 sensor
  S8_serial.begin(S8_BAUDRATE);
  sensor_S8 = new S8_UART(S8_serial);

  // Check if S8 is available
  sensor_S8->get_firmware_version(sensor.firm_version);
  int len = strlen(sensor.firm_version);
  if (len == 0) {
    Serial.println(F("SenseAir S8 CO2 sensor not found!"));
    while (1) {
      delay(1);
    };
  }

  // Show basic S8 sensor info
  Serial.println(F(">>> SenseAir S8 NDIR CO2 sensor <<<"));
  printf("Firmware version: %s\n", sensor.firm_version);
  sensor.sensor_id = sensor_S8->get_sensor_ID();
  Serial.print(F("Sensor ID: 0x")); printIntToHex(sensor.sensor_id, 4);
  Serial.println(F(""));
  Serial.println(F("Setup done!"));
  Serial.flush();

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
  sensor.co2 = sensor_S8->get_co2();
  Serial.print(F("CO2: "));
  Serial.print(sensor.co2);
  Serial.println(F(" ppm"));

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  u8g2.setCursor(0, 12);
  u8g2.print(F("CO2: "));
  u8g2.print(sensor.co2);
  u8g2.print(F(" ppm"));
  u8g2.sendBuffer();
  
  delay(5000);
}
