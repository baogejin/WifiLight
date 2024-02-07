#ifndef ROM_DATA_H
#define ROM_DATA_H

#include <EEPROM.h>
void RomSaveString(int pos, String str) {
  int len = str.length();
  EEPROM.write(pos, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(pos + i + 1, str[i]);
  }
  EEPROM.commit();
}

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

void ResetData() {
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  ESP.restart();
}

#endif