#include "storage.h"

#define STG_CLAIM_CODE_START 0
#define STG_CLAIM_CODE_LEN 32


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


  bool  write_eeprom(const char *buffer, int start, int len) {
    for(int x=0; x < len; x++) {
      int i = x + start;
      EEPROM.write(buffer[x], i);
    }

  }

  int get_claim_code(char *buffer, int len) {
    if (len < STG_CLAIM_CODE_LEN + 1) {
      return -1;
    }
    return read_eeprom(buffer, STG_CLAIM_CODE_START, STG_CLAIM_CODE_LEN);
  }

  bool set_claim_code(const char *claim_code) {
    int len = strlen(claim_code);
    if (len > STG_CLAIM_CODE_LEN) return false;
    return write_eeprom(claim_code, STG_CLAIM_CODE_START, len);
  }
};
