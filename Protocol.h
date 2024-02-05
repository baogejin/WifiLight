#include "ByteBuffer.h"

class BaseReq {
public:
  char* GetBuffer() {
    return _b.GetBuffer();
  }

  int GetSize() {
    return _b.GetSize();
  }
protected:
  ByteBuffer _b;
}

class RegisterReq : BaseReq {
public:
  RegisterReq(int itemType, String name) {
    int len = 4 + 4 + name.length();  //itemtype长度4+name长度（4+字符串长度）
    _b.WriteInt(itemType);
    _b.WriteString(name);
  }

  int GetMsgId() {
    return _msgId;
  }
private:
  int _msgId = 1;
};

class RegisterAck {
public:
  RegisterAck(char* buf, size_t size)
    : _b(buf, size) {
    Result = _b.ReadInt();
  }

  int Result;
private:
  ByteBuffer _b;
};