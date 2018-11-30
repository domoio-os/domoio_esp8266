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


void int_to_buff32(byte *buffer, int value) {
  buffer[0] = (byte) ((value >> 24) & 0xFF);
  buffer[1] = (byte) ((value >> 16) & 0xFF);
  buffer[2] = (byte) ((value >> 8) & 0xFF);
  buffer[3] = (byte) (value & 0xFF);
}


int buff32_to_int(byte *buffer) {
  return *(buffer) << 24 | *(buffer + 1) << 16 | *(buffer + 2) << 8 | *(buffer + 3);
}


void float_to_buff32(byte *buffer, float value) {
  bool isNeg = value < 0;
  if ( isNeg ) value = -value;
  int exp = 0;
  if (value != 0.0) {
    value = ldexp( frexp(value, &exp), 24);
    exp += 126 ;
  }

  uint32_t mant = value;
  buffer[0] = (byte) (((isNeg ? 0x80 : 0x00) | exp >> 1) & 0xFF);
  buffer[1] = (byte) ((((exp << 7) & 0x80) | ((mant >> 16) & 0x7F) ) & 0xFF);
  buffer[2] = (byte) ((mant >> 8) & 0xFF);
  buffer[3] = (byte) (mant & 0xFF);
}
