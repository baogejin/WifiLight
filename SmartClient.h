#ifndef SMART_CLIENT_H
#define SMART_CLIENT_H

#include <WiFiClient.h>
#include "MsgPack.h"
#include "Protocol.h"
#define MAX_CLIENT_BUFFER_SIZE 256

class SmartClient {
public:
  SmartClient() {
    _size = 0;
    _seq = 0;
  }

  void Tick();
  void SendMsg(BaseReq& msg);
  void SetName(String name) {
    _name = name;
  }
private:
  void processMsg(char* data, int len);

  WiFiClient _c;
  char _buf[MAX_CLIENT_BUFFER_SIZE];
  int _size;
  int _seq;
  MsgPack _m;
  String _name;
};

#endif