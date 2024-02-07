#ifndef MSG_PACK_H
#define MSG_PACK_H
#include "ByteBuffer.h"

class MsgPack {
public:
  void PackMsg(int seq, int msgId, char *buf, int size) {
    _b.Clear();
    _b.WriteInt(12 + size);
    _b.WriteInt(seq);
    _b.WriteInt(msgId);
    _b.Write(buf, size);
  }

  char *GetBuffer() {
    return _b.GetBuffer();
  }

  int GetSize() {
    return _b.GetSize();
  }
private:
  ByteBuffer _b;
};

#endif