/*
   Using  ESP32 + SSD1306
   Detect NTP Time
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <Timezone.h>

// ESP32 SCL=22 SDA=21
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Define NTP properties
#define NTP_OFFSET   60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "asia.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)

// Set up the NTP UDP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

const char* ssid = "";
const char* password = "";
String displaydate;
String displaytime;
const char * days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"} ;
const char * months[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"} ;
const char * ampm[] = {"AM", "PM"} ;

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.setFont(u8g2_font_ncenB08_tr);
  // Connect to wifi
  u8g2.clearBuffer();
  u8g2.drawStr(0, 16, "Connecting to WiFi...");
  u8g2.sendBuffer();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  u8g2.drawStr(0, 32, "Connected.");
  u8g2.setCursor(0, 48);
  u8g2.print("IP:");
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  timeClient.begin();   // Start the NTP UDP client
  delay(1000);
}

void loop() {
  // update the NTP client and get the UNIX UTC timestamp
  timeClient.update();
  unsigned long epochTime =  timeClient.getEpochTime();
  // convert received time stamp to time_t object
  time_t local, utc;
  utc = epochTime;

  // Then convert the UTC UNIX timestamp to local time
  TimeChangeRule TaipeiEDT = {"EDT", Second, Sun, Mar, 2, +420};  //UTC - 5 hours - change this as needed
  TimeChangeRule TaipeiEST = {"EST", First, Sun, Nov, 2, +420};   //UTC - 6 hours - change this as needed
  Timezone TaipeiAsia(TaipeiEDT, TaipeiEST);
  local = TaipeiAsia.toLocal(utc);
  //long local2 = local - 946684800;
  //Serial.println(local2);

  displaydate = "";
  displaytime = "";

  displaydate += months[month(local) - 1];
  displaydate += "/";
  if (day(local) < 10) // add a zero if day is under 10
    displaydate += "0";
  displaydate += day(local);
  displaydate += " ";
  displaydate += days[weekday(local) - 1];

  displaytime += hour(local);
  displaytime += ":";
  if (minute(local) < 10) // add a zero if minute is under 10
    displaytime += "0";
  displaytime += minute(local);
  displaytime += ":";
  if (second(local) < 10) // add a zero if second is under 10
    displaytime += "0";
  displaytime += second(local);

  u8g2.clearBuffer();
  u8g2.setCursor(67, 10);
  u8g2.setFont(u8g2_font_t0_11b_me);
  u8g2.print(displaydate);
  u8g2.setCursor(67, 23);
  u8g2.setFont(u8g2_font_tenthinguys_tn);
  u8g2.print(displaytime);
  u8g2.sendBuffer();
}
