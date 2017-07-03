#ifndef STORAGE_H
#define STORAGE_H

#include <EEPROM.h>

namespace Storage {

  void begin();

  int get_device_id(char *buffer, int len);
  bool set_device_id(const char *device_id);

};

#endif //STORAGE_H
