#ifndef DOMOIO_H
#define DOMOIO_H
#include "Arduino.h"


void domoio_setup();
void domoio_loop();

/*
 * ports
 */


enum PortType {
  PORT_DIGITAL,
  PORT_ANALOGIC
};

enum PortIO {
  PORT_INPUT,
  PORT_OUTPUT
};

class Port {
 public:
  int id;
  int value;
  PortType port_type;
  PortIO io;
  bool active_low;

  void set_value(int value);
};

typedef enum {
  EVENT_CONNECTED,
  EVENT_DISCONNECTED,
  EVENT_NEW_CONFIG
} event_type;


void setup_port(Port*);
Port * get_port(int port_id);
void set_port(int port_id, int value);

void custom_setup();
void reset_btn_callback();
void handle_event(event_type type, void * payload);

int request_config();

/*
 * Messages
 */


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

  boolean binary(byte *buffer);
 private:
  int value;
};


class FloatMessageValue : public MessageValue {
 public:
  FloatMessageValue(int port_id, float value) {
    this->port_id = port_id;
    this->value = value;
  }

  boolean binary(byte *buffer);

 private:
  float value;
};


void remote_log(const char* msg);
void send_port_change(MessageValue *port_value);
void send_ports_change(MessageValue *ports_values[], int length);
int send_json(int, const char*);


/*
 * Logging
 */


#ifdef SERIAL_LOG

#define PRINT(...) Serial.printf(__VA_ARGS__)
#define PRINTLN(str) Serial.println(str)

#else

#define PRINT(...) ((void)0)
#define PRINTLN(str) ((void)0)

#endif


#endif //DOMOIO_H
