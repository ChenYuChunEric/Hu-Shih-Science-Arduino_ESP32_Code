/*
   Using  ESP32 + SSD1306 + DS1302
   Detect RTC Time
   CLK/SCLK: 14
   DAT/IO  : 12
   RST/CE  : 13
   VCC     : 5v
   GND     : GND
   Note    : Before using this program, you should check when was your computer's start time
             Becuz mine is 2000/01/01 00:00:00, but NTP is 1970/01/01 00:00:00
             So there is 946684800 seconds between these two timecounter
             Make sure yours and change the number below, called "timetostart"
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(12, 14, 13); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Define NTP properties
#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "asia.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)
#define timetostart 946684800

// Set up the NTP UDP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

const char* ssid = "";
const char* password = "";

void setup ()
{
  Serial.begin(115200);
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

  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.setFont(u8g2_font_ncenB08_tr);
  // Connect to wifi
  u8g2.clearBuffer();
  u8g2.drawStr(0, 16, "Connecting to WiFi...");
  u8g2.sendBuffer();
  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(F("Can't connect to WiFi"));
    delay(2000);
  }
  else {
    u8g2.drawStr(0, 32, "Connected.");
    u8g2.setCursor(0, 48);
    u8g2.print("IP:");
    u8g2.print(WiFi.localIP());
    u8g2.sendBuffer();
    delay(1000);
  }
  timeClient.begin();   // Start the NTP UDP client
}

void loop ()
{
  RtcDateTime now;
  timeClient.update();
  unsigned long epochTime =  timeClient.getEpochTime();
  time_t local, utc;
  utc = epochTime;
  TimeChangeRule TaipeiEDT = {"EDT", Second, Sun, Mar, 2, +420};  //UTC - 5 hours - change this as needed
  TimeChangeRule TaipeiEST = {"EST", First, Sun, Nov, 2, +420};   //UTC - 6 hours - change this as needed
  Timezone TaipeiAsia(TaipeiEDT, TaipeiEST);
  local = TaipeiAsia.toLocal(utc);
  local = local - timetostart;

  u8g2.clearBuffer();
  if (WiFi.status() != WL_CONNECTED) {
    u8g2.setCursor(0, 12);
    u8g2.setFont(u8g2_font_t0_11b_me);
    u8g2.print(F("Offline"));
    Serial.println(F("Offline"));
    now = Rtc.GetDateTime();
  }
  else {
    u8g2.setCursor(0, 12);
    u8g2.setFont(u8g2_font_t0_11b_me);
    u8g2.print(F("Online"));
    Serial.println(F("Online"));
    now = local;
  }

  String showdate = "";
  String showtime = "";

  showdate = now.Year();
  showdate += "/";
  if (now.Month() < 10) {
    showdate += 0;
  }
  showdate += now.Month();
  showdate += "/";
  if (now.Day() < 10) {
    showdate += 0;
  }
  showdate += now.Day();

  if (now.Hour() < 10) {
    showtime += 0;
  }
  showtime = now.Hour();
  showtime += ":";
  if (now.Minute() < 10) {
    showtime += 0;
  }
  showtime += now.Minute();
  showtime += ":";
  if (now.Second() < 10) {
    showtime += 0;
  }
  showtime += now.Second();

  Serial.print(showdate);
  Serial.print("  ");
  Serial.println(showtime);


  u8g2.setCursor(0, 26);
  u8g2.setFont(u8g2_font_t0_11b_me);
  u8g2.print(showdate);
  u8g2.setCursor(0, 40);
  u8g2.setFont(u8g2_font_tenthinguys_tn);
  u8g2.print(showtime);
  u8g2.sendBuffer();

  delay(1000);
}
