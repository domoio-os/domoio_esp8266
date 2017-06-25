#ifndef DOMOIO_H
#define DOMOIO_H

void delete_credentials();
void reset();

/*
 * messages
 */

enum ActionType {
  ACTION_LOG
};

class Message {
  ActionType action;
  byte *payload;
  int payload_len;
  bool confirmable;
public:
  Message(ActionType _action, const char* msg) : action(_action), payload((byte *)msg), payload_len(strlen(msg)) {}
  int send();
};

bool is_connected();
void connect();
void receive_messages();
int send(const void* data, int size);




void remote_log(const char* msg);
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
