/*
   Using ESP32 + SD Module
   Read & Write Data
   SS(CS)   - pin 5
   MOSI     - pin 23
   MISO     - pin 19
   CLK(SCK) - pin 18
   VCC      - 5V
   GND      - GND

   SD.begin（CS,MOSI,MISO,SCK）
*/

#include <WiFi.h>
#include <SPI.h>
#include <mySD.h>
#include <U8g2lib.h>

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

ext::File myFile;

#define filename "Eric2.txt"

int a = 0;

void setup()
{
  Serial.begin(115200);
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(5, 23, 19, 18)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));

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
  myFile = SD.open(filename, FILE_WRITE);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  if (myFile) {
    Serial.print(F("Writing......"));
    u8g2.setCursor(0, 12);
    u8g2.print(F("Writing......"));
    myFile.println(a);
    myFile.close();
    u8g2.setCursor(0, 26);
    u8g2.print(a);
    Serial.println(F("Done."));
    u8g2.setCursor(0, 40);
    u8g2.print(F("Done."));
    u8g2.sendBuffer();
  } else {
    Serial.println(F("Error Opening"));
    u8g2.setCursor(0, 54);
    u8g2.print(F("Error Opening"));
    u8g2.sendBuffer();
  }

  myFile = SD.open(filename);
  if (myFile) {
    Serial.println(filename);

    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println(F("Error Opening"));
  }
  a += 1;
  delay(5000);
}
