#include "SmartClient.h"
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
      _size += readLen;
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
  if (!_c.connected() && _host != "" && _port > 0 && _port <= 65535) {
    unsigned long currentTime = millis();
    if (_lastConnectTime == 0 || currentTime - _lastConnectTime >= TRY_CONNECT_SERVER_TIME) {
      //每分钟会尝试连接服务器
      _lastConnectTime = currentTime;
      Serial.printf("尝试连接服务器 %s:%d\n", _host.c_str(), _port);
      if (_c.connect(_host, _port)) {
        //连接上后清空本地缓存，进行注册
        Serial.println("已连接上服务器");
        _size = 0;
        _seq = 0;
        //向服务器注册本设备
        RegisterReq req(1, _name);
        SendMsg(req);
        Serial.println("注册消息已发送");
      } else {
        Serial.println("连接服务器失败");
      }
    }
  }
}

void SmartClient::processMsg(char* data, int len) {
  int seq = *((int*)(data + 4));
  int msgId = *((int*)(data + 8));
  switch (msgId) {
    case MsgId_RegisterAck:
      {
        RegisterAck msg(data + 12, len);
        if (msg.Ret == 0) {
          Serial.println("设备注册成功");
        } else {
          Serial.printf("设备注册失败,错误id:%d\n", msg.Ret);
        }
        reportStatus();
      }
      break;
    case MsgId_ReportStatusAck:
      {
        ReportStatusAck msg(data + 12, len);
        if (msg.Ret == 0) {
          Serial.println("上报状态成功");
        } else {
          Serial.printf("上报状态失败,错误id:%d\n", msg.Ret);
        }
      }
      break;
    case MsgId_ChangeStatusPush:
      {
        ChangeStatusPush msg(data + 12, len);
        changeStatus(msg.Status);
      }
      break;
    default:
      Serial.printf("消息id:%d没有找到,丢弃消息\n", msgId);
      break;
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

void SmartClient::SwitchStatus() {
  _status = _status ? 0 : 1;
  digitalWrite(_pinId, _status ? HIGH : LOW);
  reportStatus();
}

void SmartClient::changeStatus(int status) {
  _status = status ? 1 : 0;
  digitalWrite(_pinId, _status ? HIGH : LOW);
  reportStatus();
}

void SmartClient::reportStatus() {
  ReportStatusReq req(_status);
  SendMsg(req);
}