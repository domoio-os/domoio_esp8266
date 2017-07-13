#ifndef DOMOIO_H
#define DOMOIO_H


#define SERIAL_LOG
#ifdef SERIAL_LOG

#define PRINT(str) Serial.print(str)
#define PRINTLN(str) Serial.println(str)

#else

#define PRINT(str) ((void)0)
#define PRINTLN(str) ((void)0)

#endif



void delete_credentials();
void reset();

/*
 * messages
 */

enum ActionType {
  ACTION_LOG
};

enum ReactAction {
  REACT_CONNECTING_WIFI,
  REACT_CONNECTING_DOMOIO,
  REACT_CONNECTED,
  REACT_FLASHING,
  REACT_AP_SERVER,
  REACT_AP_RESET_WIFI_CONFIG
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
bool register_device(String claim_code, String public_key);



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

int decrypt_hex(const char *src, char *out, int len);
int decrypt(const byte *src, byte *out, int len);

#endif //DOMOIO_H
