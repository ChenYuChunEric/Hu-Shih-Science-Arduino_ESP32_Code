#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <U8g2lib.h>
#include <Arduino.h>

/*---------- WiFi Setting ----------*/
char* ssid = "";
char* password = "";
/*---------- WiFi Setting ----------*/

/*---------- MQTT Setting ----------*/
char* MQTTServer = "";    // 本機 IPv4 位址
int MQTTPort = 1883;                  // 使用 Port
char* MQTTUser = "";                  // 設定帳號(可選)
char* MQTTPassword = "";              // 設定密碼(可選)
char* MQTTPubTopic1 = ""; // 推播主題1:傳送資料
char* MQTTSubTopic1 = "";  // 訂閱主題1:接收資料
long MQTTLastPublishTime;             // 此變數用來記錄推播時間
long MQTTPublishInterval = 10000;     // 每10秒推撥一次
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);
/*---------- MQTT Setting ----------*/

/*---------- SSD1306 ----------*/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); // ESP32 SCL=22 SDA=21
/*---------- SSD1306 ----------*/

void setup() {
  /*---------- Begin ----------*/
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();
  /*---------- Begin ----------*/

  /*---------- U8g2&SSD1306 ----------*/
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 16, "Connecting to WiFi...");
  u8g2.sendBuffer();
  /*---------- U8g2&SSD1306 ----------*/

  /*---------- WiFi ----------*/
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  u8g2.drawStr(0, 32, "Connected."); // U8g2&SSD1306
  u8g2.setCursor(0, 48);
  u8g2.print(F("IP:"));
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  /*---------- WiFi ----------*/

  /*---------- MQTT ----------*/
  MQTTConnected();
  /*---------- MQTT ----------*/
}

void loop() {
  
  /*---------- Put data into Json form ----------*/
  JSONVar myObject;
  myObject["PMS5003T"][0] = pms.pm01;
  myObject["PMS5003T"][1] = pms.pm25;
  myObject["PMS5003T"][2] = pms.pm10;
  String pushtomqtt = JSON.stringify(myObject);
  /*---------- Put data into Json form ----------*/

  /*---------- If disconnect WiFi or MQTT ----------*/
  if (WiFi.status() != WL_CONNECTED) { // 如果WiFi連線中斷，則重啟WiFi連線
    WiFi.begin(ssid, password);
  }
  if (!MQTTClient.connected()) {       // 如果MQTT連線中斷，則重啟MQTT連線
    MQTTConnected();
  }
  /*---------- If disconnect WiFi or MQTT ----------*/

  /*---------- MQTT Send Data & Use Millis to count time ----------*/
  if ((millis() - MQTTLastPublishTime) >= MQTTPublishInterval ) {
    MQTTClient.publish(MQTTPubTopic1, String(pushtomqtt).c_str());
    Serial.println(F("已推播到MQTT Broker"));
    MQTTLastPublishTime = millis(); // 更新最後傳輸時間
  }
  /*---------- MQTT Send Data & Use Millis to count time ----------*/

  MQTTClient.loop();                // 更新訂閱狀態
  delay(50);
}

/*---------- MQTT Connected ----------*/
void MQTTConnected() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
  while (!MQTTClient.connected()) {
    //以亂數為ClietID
    String  MQTTClientid = "esp32-" + String(random(1000000, 9999999));
    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) {
      //連結成功，顯示「已連線」。
      Serial.println(F("MQTT已連線"));
      //訂閱SubTopic1主題
      MQTTClient.subscribe(MQTTSubTopic1);
    } else {
      //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print(F("MQTT連線失敗,狀態碼="));
      Serial.println(MQTTClient.state());
      Serial.println(F("五秒後重新連線"));
      delay(5000);
    }
  }
}
/*---------- MQTT Connected ----------*/

/*---------- MQTT Get Data & React ----------*/
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic); Serial.print("訂閱通知:");
  String payloadString;//將接收的payload轉成字串
  //顯示訂閱內容
  for (int i = 0; i < length; i++) {
    payloadString = payloadString + (char)payload[i];
  }
  Serial.println(payloadString);
  //比對主題是否為訂閱主題1
  if (strcmp(topic, MQTTSubTopic1) == 0) {
    Serial.println("改變燈號：" + payloadString);
    if (payloadString == "ON") {

    }
    if (payloadString == "OFF") {

    }
  }
}
/*---------- MQTT Get Data & React ----------*/
