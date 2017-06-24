#ifndef DOMOIO_H
#define DOMOIO_H

void delete_credentials();
void reset();

/*
 * messages
 */

bool is_connected();
void connect();
void receive_messages();
int send(const void* data, int size);


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
