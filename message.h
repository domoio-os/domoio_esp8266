#ifndef MESSAGE_H
#define MESSAGE_H

#import "tools.h"

int next_message_id();

enum ActionType {
  ACTION_LOG,
  ACTION_SET_PORT,
  ACTION_SET_PORTS
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
 Message(ActionType _action, byte* msg, int length) : action(_action), payload(msg), payload_len(length) {}
  int send();
};

bool is_connected();
void connect();
void disconnect();
void receive_messages();
int send(const void* data, int size);
bool register_device(String claim_code, String public_key);
bool is_ota_requested();
bool is_reconnect_requested();
void ota_update();

class MessageValue {
public:
  virtual boolean binary(byte *buffer) { return false; }
  int port_id;
};

class IntMessageValue : public MessageValue {
public:
  IntMessageValue(int port_id, int value) {
    this->port_id = port_id;
    this->value = value;
  }

  boolean binary(byte *buffer) {
    i2buff(buffer, value);
    return true;
  };
private:
  int value;
};



void remote_log(const char* msg);
void send_port_change(int port_id, int value);
void send_ports_change(MessageValue *ports_values[], int length);


#endif //MESSAGE_H
