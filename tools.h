#ifndef TOOLS_H
#define TOOLS_H


int buff2i(byte *buf, int offset=0);
void i2buff(byte *buffer, int value);



void int_to_buff32(byte *buffer, int value);
void float_to_buff32(byte *buffer, float value);

#endif //TOOLS_H
