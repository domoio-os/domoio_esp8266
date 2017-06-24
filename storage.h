#ifndef STORAGE_H
#define STORAGE_H

#include <EEPROM.h>

namespace Storage {

  void begin();

  int get_claim_code(char *buffer, int len);
  bool set_claim_code(const char *claim_code);

};

#endif //STORAGE_H
