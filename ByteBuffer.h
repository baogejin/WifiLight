#define MAX_BUFFER_SIZE 128
class ByteBuffer {
public:
  ByteBuffer(char *buf, size_t size) {
    _size = size;
    if (_size > MAX_BUFFER_SIZE) {
      _size = MAX_BUFFER_SIZE;
    }
    _pos = 0;
    memcpy(_buf, buf, size);
  }

  ByteBuffer() {
    _size = 0;
    _pos = 0;
    memset(_buf, 0, sizeof(_buf));
  }

  char *GetBuffer() {
    return _buf;
  }

  int GetSize() {
    return _pos;
  }

  int ReadInt() {
    if (_pos + 4 > _size) {
      Serial.println("ReadInt buf size err");
      return 0;
    }
    int ret = *((int *)(&(_buf[_pos])));
    _pos += 4;
    return ret;
  }

  void WriteInt(int v) {
    if (_pos + 4 > MAX_BUFFER_SIZE) {
      Serial.println("WriteInt buf size err");
      return;
    }
    memcpy(&_buf[_pos], &v, 4);
    _pos += 4;
  }

  String ReadString() {
    String ret = "";
    int len = ReadInt();
    if (_pos + len > _size) {
      Serial.println("ReadString buf size err");
      return ret;
    }
    for (int i = 0; i < len; i++) {
      ret += _buf[_pos + i];
    }
    _pos += len;
    return ret;
  }

  void WriteString(String s) {
    int len = s.length();
    if (_pos + 4 + len > MAX_BUFFER_SIZE) {
      return;
    }
    WriteInt(len);
    memcpy(&_buf[_pos], s.c_str(), len);
    _pos += len;
  }

private:
  size_t _size;
  size_t _pos;
  char _buf[MAX_BUFFER_SIZE];
};