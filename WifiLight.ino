/*
wifi配置，需要连接开发板的ap，进入192.168.4.1网页进行设置，联网成功后，开发板的ap就将关闭
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef APSSID
#define APSSID "WifiLight"
#define APPSK "88888888"
#endif

const char *ssid = APSSID;
const char *password = APPSK;
String staSsid;
String staPassword;
bool tryConnect = false;
bool apOn = false;
bool webOn = false;

ESP8266WebServer server(80);

void setup() {
  delay(1000);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    configLoop();
  } else {
    workLoop();
  }
  delay(500);
}

void workLoop(){
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
  if (tryConnect) {
    tryConnect = false;
    WiFi.begin(staSsid, staPassword);
    Serial.println("try connect:" + staSsid + staPassword);
    int counter = 0;
    while (1) {
      counter++;
      if (counter > 20) {
        Serial.println("连接超时");
        break;
      }
      delay(1000);
      Serial.println(counter);
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("网络连接成功");
        closeWeb();
        closeAP();
        break;
      }
    }
  }
}

void startAP() {
  Serial.println("start ap");
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  apOn = true;
}

void closeAP() {
  Serial.println("close ap");
  WiFi.mode(WIFI_STA);
}

void startWeb() {
  server.on("/", handleRoot);
  server.on("/HandleConnectWifi", handleConnnect);
  server.begin();
  Serial.println("HTTP server started");
  webOn = true;
}

void closeWeb() {
  Serial.println("close web");
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
  s += "        var xmlhttp=new XMLHttpRequest();";
  s += "        xmlhttp.open(\"POST\",\"/HandleConnectWifi?ssid=\"+wifiName+\"&password=\"+password,true);";
  s += "        xmlhttp.send();";
  s += "        xmlhttp.onload = function(e){alert(this.responseText);}";
  s += "      }";
  s += "    </script>";
  s += "    <form name=\"wifiConfig\">";
  s += "      <h1>wifi灯网络设置</h1>";
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

void handleConnnect() {
  Serial.println("onConnect");
  String wifiName = server.arg("ssid");
  String wifiPassword = server.arg("password");
  Serial.println("received:" + wifiName + "," + wifiPassword);
  server.send(200, "text/html", "连接中..");
  staSsid = wifiName;
  staPassword = wifiPassword;
  tryConnect = true;
}