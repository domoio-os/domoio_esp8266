#include <Arduino.h>

int buff2i(byte *buf, int offset) {
  byte first = *(buf + offset);
  byte last = *(buf + offset + 1);
  return last | first << 8;
}
