#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
// #include <ArduinoOTA.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "cantcoap.h"
#include "domoio.h"

const char* host = "10.254.0.200";
const int port = 1234;

const char *hardware_id = "514e99d8-d2c0-4fc8-aa7d-db7cd1c7e688";
WiFiClientSecure client;

#define BUFFER_SIZE 512

byte buffer[BUFFER_SIZE];


#define URI_BUFFER_LENGTH 25
void clear_buffer() {
  memset(buffer, 0, BUFFER_SIZE);
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


int send(const void* data, int size) {

  int packet_size = size + 2;
  byte buffer[packet_size];

  buffer[0] = (byte) ((size >> 8) & 0xFF);
  buffer[1] = (byte) (size & 0xFF);

  memcpy(&buffer[2], data, size);

  return client.write(&buffer[0], packet_size);
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

  // CoapPDU *reply = new CoapPDU();

	// reply->setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
	// reply->setCode(msg->getCode());
	// reply->setToken(msg->getTokenPointer(), msg->getTokenLength());
	// reply->setMessageID(msg->getMessageID());
  // send(reply->getPDUPointer(), reply->getPDULength());
  // delete(reply);
}


void connect_wifi() {
  WiFiManager wifiManager;

  if (!wifiManager.autoConnect("domoio_123")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
}


bool handsake() {
  send(hardware_id, strlen(hardware_id));
  //  String line = client.readStringUntil(0);
  int size = block_until_receive();

  Serial.print("Received reply to handsake ");
  Serial.println(size);
  Serial.print(" content ");
  Serial.println((char *)&buffer[0]);

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



void network_loop() {
  int size = receive();

  if (size <= 0) {
    return;
  }

  Serial.print("Received: ");
  Serial.println(size);

  CoapPDU coap_msg = CoapPDU(buffer, size);
  // coap_packet_t pkt;

  if (!coap_msg.validate()) {
    Serial.print("Bad packet rc");
    return;
  }

  process_message(&coap_msg);
}


void setup() {
  // WiFi.mode(WIFI_OFF);

  Serial.begin(115200);

  connect_wifi();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // ArduinoOTA.begin();
}

void loop() {
  if (!client.connected()) {
    connect();
    return;
  }


  network_loop();

  // serial_loop();
}
