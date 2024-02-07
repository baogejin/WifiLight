#include "SmartClient.h"

const char* SERVER_IP = "192.168.31.136";
const int SERVER_PORT = 6502;
const int MAX_SEQ = 99999;
void SmartClient::Tick() {
  //消息读取
  if (int n = _c.available()) {
    int readLen = n;
    //读取长度限制
    if (_size + n > MAX_CLIENT_BUFFER_SIZE) {
      readLen = MAX_CLIENT_BUFFER_SIZE - _size;
    }
    if (readLen > 0) {
      _c.read(_buf + _size, readLen);
      if (_size > 4) {
        int needLen = *((int*)_buf);
        //当缓冲区有完整长度的包的时候，进行消息处理
        if (_size >= needLen) {
          processMsg(_buf, needLen);
          memcpy(_buf, _buf + needLen, _size - needLen);
          _size -= needLen;
        }
      }
    }
  }
  //连接检查
  if (!_c.connected()) {
    if (_c.connect(SERVER_IP, SERVER_PORT)) {
      //连接上后清空本地缓存，进行注册
      Serial.println("已连接上服务器");
      _size = 0;
      _seq = 0;
      //todo register
      RegisterReq req(1, _name);
      SendMsg(req);
      Serial.println("注册消息已发送");
    }
  }
}

void SmartClient::processMsg(char* data, int len) {
  int msgId = *((int*)(data + 4));
  int seq = *((int*)(data + 8));
  switch (msgId) {
    default:
      Serial.printf("消息id:%d没有找到,丢弃消息\n", msgId);
  }
}

void SmartClient::SendMsg(BaseReq& msg) {
  _seq++;
  if (_seq > MAX_SEQ) {
    _seq = _seq % MAX_SEQ;
  }
  _m.PackMsg(_seq, msg.GetMsgId(), msg.GetBuffer(), msg.GetSize());
  _c.write(_m.GetBuffer(), _m.GetSize());
}