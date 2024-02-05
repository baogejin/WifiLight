/*
wifi配置，需要连接开发板的ap，进入192.168.4.1网页进行设置，联网成功后，开发板的ap就将关闭
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "RomData.h"

#ifndef APSSID
#define APSSID "WifiLight"
#define APPSK "88888888"
#define SSID_POS 10
#define PASSWORD_POS 100
#define NAME_POS 150
#endif

const char *ssid = APSSID;
const char *password = APPSK;
bool apOn = false;
bool webOn = false;
String name;

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("");
  Serial.println("-----------------------------------------");
  Serial.println("程序开始运行啦");
  Serial.println("-----------------------------------------");
  EEPROM.begin(512);
  String savedSsid = RomReadString(SSID_POS);
  String savedPassword = RomReadString(PASSWORD_POS);
  name = RomReadString(NAME_POS);
  if (savedSsid.length() > 0 && savedPassword.length() > 0) {
    wifiConnect(savedSsid, savedPassword);
  }
  if (name.length() > 0) {
    Serial.println("读取到设备名称:" + name);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(15, INPUT);
}

void loop() {
  if (digitalRead(15) == HIGH) {
    ResetData();
    return;
  }
  if (WiFi.status() != WL_CONNECTED) {
    configLoop();
  } else {
    workLoop();
  }
  delay(500);
}

void workLoop() {
  digitalWrite(LED_BUILTIN, LOW);
  //todo
}

void configLoop() {
  digitalWrite(LED_BUILTIN, HIGH);
  if (!apOn) {
    startAP();
  }
  if (!webOn) {
    startWeb();
  }
  server.handleClient();
}

void startAP() {
  Serial.println("wifi接入点开启");
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  apOn = true;
}

void closeAP() {
  Serial.println("wifi接入点关闭");
  WiFi.mode(WIFI_STA);
}

void startWeb() {
  server.on("/", handleRoot);
  server.on("/HandleConnectWifi", handleConnect);
  server.begin();
  Serial.println("http服务开启");
  webOn = true;
}

void closeWeb() {
  Serial.println("http服务关闭");
  server.close();
}

void handleRoot() {
  String s = "";
  s += "<html>";
  s += "  <head>";
  s += "    <meta charset=\"UTF-8\">";
  s += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  s += "    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">";
  s += "    <title>wifi灯网络设置</title>";
  s += "  </head>";
  s += "  <body>";
  s += "    <script type=\"text/javascript\">";
  s += "      function connectWifi() {";
  s += "        var selectObj = document.getElementById(\"wifiName\");";
  s += "        var nameIdx = selectObj.selectedIndex;";
  s += "        var wifiName = selectObj[nameIdx].value;";
  s += "        var password = wifiPassword.value;";
  s += "        var nameValue = nameinput.value;";
  s += "        var xmlhttp=new XMLHttpRequest();";
  s += "        xmlhttp.open(\"POST\",\"/HandleConnectWifi?ssid=\"+wifiName+\"&password=\"+password+\"&name=\"+nameValue,true);";
  s += "        xmlhttp.send();";
  s += "        xmlhttp.onload = function(e){alert(this.responseText);}";
  s += "      }";
  s += "    </script>";
  s += "    <form name=\"wifiConfig\">";
  s += "      <h1>wifi灯网络设置</h1>";
  s += "      设备名称:<input id=\"nameinput\" placeholder=\"" + name + "\">";
  s += "      <br>";
  s += "      wifi名称:" + getWifiList();
  s += "      <br>";
  s += "      wifi密码:<input id=\"wifiPassword\">";
  s += "      <br>";
  s += "      <button onclick=\"connectWifi()\">连接</button>";
  s += "    </form>";
  s += "  </body>";
  s += "</html>";
  server.send(200, "text/html", s);
}

String getWifiList() {
  String list = "";
  int scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/false);
  for (int8_t i = 0; i < scanResult; i++) {
    String wifiName = WiFi.SSID(i);
    list += "<option>" + wifiName + "</option>";
  }
  return "<select id=\"wifiName\"" + list + "</select>";
}

void handleConnect() {
  String wifiName = server.arg("ssid");
  String wifiPassword = server.arg("password");
  name = server.arg("name");
  Serial.println("接收到wifi设置信息:" + wifiName + "," + wifiPassword);
  Serial.println("设备名称设置:" + name);
  server.send(200, "text/html", "连接中..");
  wifiConnect(wifiName, wifiPassword);
}

void wifiConnect(String ssid, String password) {
  WiFi.begin(ssid, password);
  Serial.println("尝试连接wifi:" + ssid + "," + password);
  WiFi.waitForConnectResult(20000);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("wifi连接成功");
    RomSaveString(SSID_POS, WiFi.SSID());
    RomSaveString(PASSWORD_POS, WiFi.psk());
    RomSaveString(NAME_POS, name);
    closeWeb();
    closeAP();
  } else {
    Serial.println("wifi连接超时");
  }
}
