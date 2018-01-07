#include <Arduino.h>
#include "storage.h"

#define STG_DEVICE_ID_START 0
#define STG_DEVICE_ID_LEN 36


namespace Storage {

  void begin() {
    EEPROM.begin(512);
  }

  int read_eeprom(char *buffer, int start, int len) {
    char c;
    for(int x=0; x < len; x++) {
      int i = x + start;
      c = EEPROM.read(i);
      buffer[x] = c;
    }
    return len;
  }


  bool  write_eeprom(const char *buffer, int start, int len) {
    for(int x=0; x < len; x++) {
      int i = x + start;
      EEPROM.write(i, buffer[x]);
    }
    EEPROM.commit();
  }

  int get_device_id(char *buffer, int len) {
    if (len < STG_DEVICE_ID_LEN + 1) {
      return -1;
    }
    read_eeprom(buffer, STG_DEVICE_ID_START, STG_DEVICE_ID_LEN);
    buffer[len] = '\0';
    return len;
  }

  bool set_device_id(const char *device_id) {
    int len = strlen(device_id);
    if (len > STG_DEVICE_ID_LEN) return false;
    return write_eeprom(device_id, STG_DEVICE_ID_START, len);
  }
};
