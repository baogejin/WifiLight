#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "ByteBuffer.h"
#include "MsgId.h"

class BaseReq {
public:
  char* GetBuffer() {
    return _b.GetBuffer();
  }

  int GetSize() {
    return _b.GetSize();
  }

  virtual int GetMsgId() = 0;
protected:
  ByteBuffer _b;
};

class RegisterReq : public BaseReq {
public:
  RegisterReq(int itemType, String name) {
    int len = 4 + 4 + name.length();  //itemtype长度4+name长度（4+字符串长度）
    _b.WriteInt(itemType);
    _b.WriteString(name);
  }

  int GetMsgId() {
    return MsgId_RegisterReq;
  }
private:
};

class RegisterAck {
public:
  RegisterAck(char* buf, size_t size)
    : _b(buf, size) {
    Ret = _b.ReadInt();
  }

  int Ret;
private:
  ByteBuffer _b;
};

#endif