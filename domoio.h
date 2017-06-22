#ifndef DOMOIO_H
#define DOMOIO_H


/*
 * Tools
 */


int buff2i(byte *buf, int offset);


/*
 * Serial
 */


void serial_loop();


/*
 * crypto
 */

int decrypt(const char *src, char *out, int len);


#endif //DOMOIO_H
