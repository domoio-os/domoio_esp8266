#include "domoio.h"
#include "message.h"
#include "cantcoap.h"
#include "tools.h"

int send_json(int port_id, const char* json) {
  char uri[30];
  snprintf(&uri[0], 30, "/json/%d", port_id);

  CoapPDU msg;
  msg.setType(CoapPDU::COAP_CONFIRMABLE);
  msg.setCode(CoapPDU::COAP_POST);
  msg.setURI(&uri[0]);
  msg.setMessageID(next_message_id());
  msg.setPayload((uint8_t*) json, strlen(json));
  return ::send(msg.getPDUPointer(), msg.getPDULength());
}

int Message::send() {
  CoapPDU msg;

  // LOG
  if (this->action == ACTION_LOG) {
    msg.setType(CoapPDU::COAP_CONFIRMABLE);
    msg.setCode(CoapPDU::COAP_POST);
    msg.setURI("/log");
  }

  if (this->action == ACTION_SET_PORT) {
    msg.setType(CoapPDU::COAP_CONFIRMABLE);
    msg.setCode(CoapPDU::COAP_POST);
    msg.setURI("/set_port");
  }

  if (this->action == ACTION_SET_PORTS) {
    msg.setType(CoapPDU::COAP_CONFIRMABLE);
    msg.setCode(CoapPDU::COAP_POST);
    msg.setURI("/set_ports");
  }

  // Default settings
  else {
    msg.setType(CoapPDU::COAP_NON_CONFIRMABLE);
    msg.setCode(CoapPDU::COAP_POST);
  }


  msg.setMessageID(next_message_id());
  msg.setPayload(this->payload, this->payload_len);

  return ::send(msg.getPDUPointer(), msg.getPDULength());
}



void remote_log(const char *data) {
  Message msg(ACTION_LOG, data);
  msg.send();
}

void send_port_change(MessageValue *port_value) {
  byte buffer[MESSAGE_VALUE_LENGTH];
  port_value->binary(&buffer[0]);
  Message msg(ACTION_SET_PORT, &buffer[0], MESSAGE_VALUE_LENGTH);
  msg.send();
}


void send_ports_change(MessageValue *port_values[], int ports_length) {
  int length = ports_length * MESSAGE_VALUE_LENGTH;
  byte buffer[length];

  for (int i=0; i < ports_length; i++) {
    int offset = i * MESSAGE_VALUE_LENGTH;
    port_values[i]->binary(&buffer[offset]);
  }

  Message msg(ACTION_SET_PORTS, &buffer[0], length);
  msg.send();
}


boolean IntMessageValue::binary(byte *buffer) {
  i2buff(buffer, this->port_id);
  *(buffer + 2) = VALUE_TYPE_INT;
  int_to_buff32(buffer + 3, this->value);
  return true;
}


boolean FloatMessageValue::binary(byte *buffer) {
  i2buff(buffer, this->port_id);
  *(buffer + 2) = VALUE_TYPE_FLOAT;
  float_to_buff32(buffer + 3, this->value);
  return true;
}
