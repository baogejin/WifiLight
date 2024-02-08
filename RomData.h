#ifndef ROM_DATA_H
#define ROM_DATA_H

#include <EEPROM.h>
//存储字符串到rom指定位置
void RomSaveString(int pos, String str) {
  int len = str.length();
  EEPROM.write(pos, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(pos + i + 1, str[i]);
  }
  EEPROM.commit();
}

//从rom指定位置读取字符串
String RomReadString(int pos) {
  String str = "";
  int len = EEPROM.read(pos);
  if (len > 30) {
    return str;
  }
  for (int i = 0; i < len; i++) {
    str += char(EEPROM.read(pos + i + 1));
  }
  return str;
}

//清除rom存储信息
void ResetData() {
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
}

#endif