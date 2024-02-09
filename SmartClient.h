#ifndef SMART_CLIENT_H
#define SMART_CLIENT_H

#include <WiFiClient.h>
#include "MsgPack.h"
#include "Protocol.h"
#define MAX_CLIENT_BUFFER_SIZE 256
#define TRY_CONNECT_SERVER_TIME 60000 //尝试连接服务器的时间间隔，单位毫秒


class SmartClient {
public:
  SmartClient(int pinId) {
    _size = 0;
    _seq = 0;
    _status = 0;
    _pinId = pinId;
    _lastConnectTime = 0;
    _host = "";
    _port = 0;
  }

  void Tick();
  void SendMsg(BaseReq& msg);
  void SetName(String name) {
    _name = name;
  }
  void SetServer(String host,int port){
    _host = host;
    _port = port;
  }
  void SwitchStatus();
private:
  void processMsg(char* data, int len);
  void changeStatus(int status);
  void reportStatus();

  WiFiClient _c;
  char _buf[MAX_CLIENT_BUFFER_SIZE];
  int _size;
  int _seq;
  MsgPack _m;
  String _name;
  int _status;
  int _pinId;
  unsigned long _lastConnectTime;
  String _host;
  int _port;
};

#endif