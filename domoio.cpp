#include <Arduino.h>
#include "domoio.h"
#include "cantcoap.h"
#include <WiFiClientSecure.h>

#define URI_BUFFER_LENGTH 25

#define BUFFER_SIZE 512


byte buffer[BUFFER_SIZE];
const char *hardware_id = "514e99d8-d2c0-4fc8-aa7d-db7cd1c7e688";
bool session_started = false;

WiFiClientSecure client;

const char* host = "10.254.0.200";
const int port = 1234;

int message_id_counter = 0;

void clear_buffer() {
  memset(buffer, 0, BUFFER_SIZE);
}

bool is_connected() {
  return client.connected();
}

int receive() {
  if (!client.available()) return -1;

  int size_buf[2];
  size_buf[0] = client.read();
  size_buf[1] = client.read();

  int size = size_buf[1] | size_buf[0] << 8;
  if (size > BUFFER_SIZE) {
    Serial.println("ERROR: OVERFLOW");
    return -1;
  }
  clear_buffer();
  for (int i=0; i < size; i++) {
    buffer[i] = client.read();
  }

  return size;
}


int block_until_receive() {
  int size;
  while((size = receive()) == -1) {
    delay(250);
  }
  return size;
}
const char *expected = "HELLO";
int expected_len = strlen(expected);

bool handsake() {
  send(hardware_id, strlen(hardware_id));
  int size = block_until_receive();
  // Serial.print("Received: ");
  // Serial.print(size);
  // Serial.print(" => ");
  // Serial.println((char *) &buffer[0]);

  if (size != expected_len || strncmp(expected, (char *) &buffer[0], size) != 0) {
    Serial.println("BAD LOGIN");
    return false;
  }

  session_started = true;
  Serial.println("Session started");

  return true;
}

void connect() {
  Serial.println("connecting");
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }

  handsake();
}




int send(const void* data, int size) {

  int packet_size = size + 2;
  byte buffer[packet_size];

  buffer[0] = (byte) ((size >> 8) & 0xFF);
  buffer[1] = (byte) (size & 0xFF);

  memcpy(&buffer[2], data, size);

  return client.write(&buffer[0], packet_size);
}

int send_confirmation(CoapPDU *msg) {
  CoapPDU reply;

	reply.setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
	reply.setCode(msg->getCode());
	reply.setToken(msg->getTokenPointer(), msg->getTokenLength());
	reply.setMessageID(msg->getMessageID());
  return send(reply.getPDUPointer(), reply.getPDULength());
}

void process_message(CoapPDU *msg) {
  char uri_buf[URI_BUFFER_LENGTH];
  int uri_size;
  msg->getURI(&uri_buf[0], URI_BUFFER_LENGTH, &uri_size);
  Serial.print("URI: ");
  Serial.println(uri_buf);


  int payload_length = msg->getPayloadLength();
  uint8_t *payload = msg->getPayloadPointer();
  Serial.print("Payload ");
  Serial.println(payload_length);

  int port_id = buff2i(payload, 0);
  int value = buff2i(payload, 2);
  Serial.print("Port: ");
  Serial.println(port_id);

  Serial.print("Value: ");
  Serial.println(value);

  if (value == 1) {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }


  // Send the response
  send_confirmation(msg);
}


void receive_messages() {
  int size = receive();

  if (size <= 0) {
    return;
  }

  // Serial.print("Received: ");
  // Serial.println(size);

  CoapPDU coap_msg = CoapPDU(buffer, size);

  if (!coap_msg.validate()) {
    Serial.print("Bad packet rc");
    return;
  }

  process_message(&coap_msg);
}



int next_message_id() {
  int id = message_id_counter++;
  return id;
}

int Message::send() {
  CoapPDU msg;

  // LOG
  if (this->action == ACTION_LOG) {
    msg.setType(CoapPDU::COAP_CONFIRMABLE);
    msg.setCode(CoapPDU::COAP_POST);
    msg.setURI("/log");
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
