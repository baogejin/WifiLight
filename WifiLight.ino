#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "RomData.h"
#include "SmartClient.h"

#ifndef APSSID
/*
wifi配置，需要连接开发板的ap，进入192.168.4.1网页进行设置，联网成功后，开发板的ap就将关闭
*/
#define APSSID "WifiLight"   //ap的名称
#define APPSK "88888888"     //ap的密码
#define SSID_POS 10          //rom存储wifi名称的位置
#define PASSWORD_POS 100     //rom存储wifi密码的位置
#define NAME_POS 150         //rom存储设备名称位置
#define RESET_DATA_PIN_ID D3  //重置按钮引脚id 5
#define SWITCH_PIN_ID D0      //设备的灯开关引脚id 3
#endif

const char *ssid = APSSID;
const char *password = APPSK;
bool apOn = false;   //ap是否开启
bool webOn = false;  //web页面是否开启
String name;         //设备名称
bool switchPressed = false;

ESP8266WebServer server(80);  //web服务器，用于配置wifi
SmartClient client;           //客户端，用于对接智能家居服务器

void setup() {
  Serial.begin(115200);  //初始化串口
  delay(1000);
  Serial.println("");
  Serial.println("-----------------------------------------");
  Serial.println("程序开始运行啦");
  Serial.println("-----------------------------------------");

  //初始化引脚
  pinMode(LED_BUILTIN, OUTPUT);              //开发板自带的led灯，不亮代表wifi没连上，亮代表wifi连上了
  pinMode(RESET_DATA_PIN_ID, INPUT_PULLUP);  //重置wifi配置输入按钮
  pinMode(SWITCH_PIN_ID, INPUT_PULLUP);      //开关按钮输入
  pinMode(LIGHT_PIN_ID, OUTPUT);             //控制照明灯泡
  digitalWrite(LIGHT_PIN_ID, LOW);

  //读取rom存储的wifi信息
  EEPROM.begin(512);
  String savedSsid = RomReadString(SSID_POS);
  String savedPassword = RomReadString(PASSWORD_POS);
  name = RomReadString(NAME_POS);
  if (savedSsid.length() > 0 && savedPassword.length() > 0) {
    //如果有存储的信息，就尝试连接wifi
    wifiConnect(savedSsid, savedPassword);
  }
  if (name.length() > 0) {
    Serial.println("读取到设备名称:" + name);
  }
}

void loop() {
  if (digitalRead(RESET_DATA_PIN_ID) == LOW) {
    //重置wifi按钮
    Serial.println("重置按钮");
    ResetData();
    ESP.restart();
    return;
  }
  if (digitalRead(SWITCH_PIN_ID) == LOW) {
    if (!switchPressed) {
      Serial.println("开关触发");
      client.SwitchStatus();
    }
    switchPressed = true;
  } else {
    if (switchPressed) {
      delay(100);
    }
    switchPressed = false;
  }
  if (WiFi.status() != WL_CONNECTED) {
    //如果没有连接wifi，则进入配置wifi的功能
    configLoop();
  } else {
    //连接了wifi，就进入正常工作流程
    workLoop();
  }
  // delay(500);
}

void workLoop() {
  digitalWrite(LED_BUILTIN, LOW);
  client.Tick();
}

void configLoop() {
  digitalWrite(LED_BUILTIN, HIGH);
  if (!apOn) {
    //没有开启ap就开启ap
    startAP();
  }
  if (!webOn) {
    //没有开启web服务就开启web服务
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

//配置wifi主页面
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

//获取wifi列表
String getWifiList() {
  String list = "";
  int scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/false);
  for (int8_t i = 0; i < scanResult; i++) {
    String wifiName = WiFi.SSID(i);
    list += "<option>" + wifiName + "</option>";
  }
  return "<select id=\"wifiName\"" + list + "</select>";
}

//处理设置wifi请求
void handleConnect() {
  String wifiName = server.arg("ssid");
  String wifiPassword = server.arg("password");
  name = server.arg("name");
  Serial.println("接收到wifi设置信息:" + wifiName + "," + wifiPassword);
  Serial.println("设备名称设置:" + name);
  server.send(200, "text/html", "连接中..");
  wifiConnect(wifiName, wifiPassword);
}

//尝试wifi连接
void wifiConnect(String ssid, String password) {
  WiFi.begin(ssid, password);
  Serial.println("尝试连接wifi:" + ssid + "," + password);
  WiFi.waitForConnectResult(20000);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("wifi连接成功");
    //存储wifi信息到rom
    RomSaveString(SSID_POS, WiFi.SSID());
    RomSaveString(PASSWORD_POS, WiFi.psk());
    RomSaveString(NAME_POS, name);
    //关闭配置wifi的ap和web
    closeWeb();
    closeAP();
    client.SetName(name);
  } else {
    Serial.println("wifi连接超时");
  }
}
