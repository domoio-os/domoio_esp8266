#include "domoio.h"
#include "cantcoap.h"

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

void send_port_change(int port_id, int value) {
  byte buffer[4];
  i2buff(&buffer[0], port_id);
  i2buff(&buffer[2], value);
  Message msg(ACTION_SET_PORT, &buffer[0], 4);
  msg.send();
}


void send_ports_change(MessageValue *port_values[], int ports_length) {
  int length = 4 * ports_length;
  byte buffer[length];

  for (int i=0; i < ports_length; i++) {
    int offset = i * 4;
    i2buff(&buffer[offset], port_values[i]->port_id);
    port_values[i]->binary(&buffer[offset + 2]);
  }

  Message msg(ACTION_SET_PORTS, &buffer[0], length);
  msg.send();
}
