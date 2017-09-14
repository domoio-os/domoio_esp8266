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

#define MESSAGE_VALUE_LENGTH 7
#define VALUE_TYPE_INT 0
#define VALUE_TYPE_FLOAT 1

class MessageValue {
public:
  virtual boolean binary(byte *buffer) { return false; }
protected:
  int port_id;
};

class IntMessageValue : public MessageValue {
public:
  IntMessageValue(int port_id, int value) {
    this->port_id = port_id;
    this->value = value;
  }

  boolean binary(byte *buffer) {
    i2buff(buffer, this->port_id);
    *(buffer + 2) = VALUE_TYPE_INT;
    int_to_buff32(buffer + 3, this->value);
    return true;
  };
private:
  int value;
};


class FloatMessageValue : public MessageValue {
public:
  FloatMessageValue(int port_id, float value) {
    this->port_id = port_id;
    this->value = value;
  }

  boolean binary(byte *buffer) {
    i2buff(buffer, this->port_id);
    *(buffer + 2) = VALUE_TYPE_FLOAT;
    float_to_buff32(buffer + 3, this->value);
    return true;
  };
private:
  float value;
};



void remote_log(const char* msg);
void send_port_change(MessageValue *port_value);
void send_ports_change(MessageValue *ports_values[], int length);


#endif //MESSAGE_H
