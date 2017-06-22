#ifndef STORAGE_H
#define STORAGE_H


#define STG_SSID_START 0
#define STG_SSID_LEN 32
#define STG_PASS_START 32
#define STG_PASS_LEN 64



namespace Storage {

  void begin() {
    EEPROM.begin(512);
  }

  int read_eeprom(char *buffer, int start, int len) {
    for(int x=0; x < len; x++) {
      int i = x + start;
      buffer[x] = EEPROM.read(i);
    }
    buffer[len] = '\0';
    return len;
  }


  int get_ssid(char *buffer, int len) {
    if (len < STG_SSID_LEN + 1) {
      return -1;
    }
    return read_eeprom(buffer, STG_SSID_START, STG_SSID_LEN);
  }






};

#endif //STORAGE_H
