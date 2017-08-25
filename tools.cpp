#include <Arduino.h>

int buff2i(byte *buf, int offset) {
  byte first = *(buf + offset);
  byte last = *(buf + offset + 1);
  return last | first << 8;
}

void i2buff(byte *buffer, int value) {
  buffer[0] = (byte) ((value >> 8) & 0xFF);
  buffer[1] = (byte) (value & 0xFF);
}
