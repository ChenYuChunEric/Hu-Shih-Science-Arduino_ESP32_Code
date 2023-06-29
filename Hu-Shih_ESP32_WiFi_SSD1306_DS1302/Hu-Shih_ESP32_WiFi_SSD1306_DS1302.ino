/*
   Using  ESP32 + SSD1306 + DS1302
   Detect RTC Time
   CLK/SCLK: 14
   DAT/IO  : 12
   RST/CE  : 13
   VCC     : 5v
   GND     : GND
*/

#include <WiFi.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <U8g2lib.h>

ThreeWire myWire(12, 14, 13); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

const char* ssid = "";
const char* password = "";

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup () {
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
  u8g2.print("IP:");
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  
  Serial.print(F("compiled: "));
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Serial.println();
  if (!Rtc.IsDateTimeValid()) {
    Serial.println(F("RTC lost confidence in the DateTime!"));
    Rtc.SetDateTime(compiled);
  }
  if (Rtc.GetIsWriteProtected()) {
    Serial.println(F("RTC was write protected, enabling writing now"));
    Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning()) {
    Serial.println(F("RTC was not actively running, starting now"));
    Rtc.SetIsRunning(true);
  }
  RtcDateTime now = Rtc.GetDateTime();
  delay(1000);
}

void loop ()
{
  RtcDateTime now = Rtc.GetDateTime();
  //Serial.println(now);
  /*Serial.print(now.Year());
  Serial.print("/");
  Serial.print(now.Month());
  Serial.print("/");
  Serial.print(now.Day());
  Serial.print("  ");
  Serial.print(now.Hour());
  Serial.print(":");
  Serial.print(now.Minute());
  Serial.print(":");
  Serial.print(now.Second());
  Serial.println();*/

  String outputdate = String(now.Year()) + "/" + now.Month() + "/" + now.Day();
  String outputtime = String(now.Hour()) + ":" + now.Minute() + ":" + now.Second();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS12_te);
  u8g2.setCursor(0, 14);
  u8g2.print(outputdate);
  u8g2.setCursor(0, 30);
  u8g2.print(outputtime);
  u8g2.sendBuffer();

  delay(1000); // ten seconds
}
