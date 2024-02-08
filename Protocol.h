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
    _b.WriteInt(itemType);
    _b.WriteString(name);
  }

  int GetMsgId() {
    return MsgId_RegisterReq;
  }
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

class ReportStatusReq : public BaseReq {
public:
  ReportStatusReq(int status) {
    _b.WriteInt(status);
  }

  int GetMsgId() {
    return MsgId_ReportStatusReq;
  }
};

class ReportStatusAck {
public:
  ReportStatusAck(char* buf, size_t size)
    : _b(buf, size) {
    Ret = _b.ReadInt();
  }

  int Ret;
private:
  ByteBuffer _b;
};

class ChangeStatusPush {
public:
  ChangeStatusPush(char* buf, size_t size)
    : _b(buf, size) {
    Status = _b.ReadInt();
  }
  int Status;
private:
  ByteBuffer _b;
};

#endif