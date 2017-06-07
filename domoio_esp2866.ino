#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
// #include <ArduinoOTA.h>


#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "coap.h"

const char* host = "10.254.0.200";
const int port = 1234;

const char *hardware_id = "514e99d8-d2c0-4fc8-aa7d-db7cd1c7e688";
WiFiClientSecure client;

#define BUFFER_SIZE 512

byte buffer[BUFFER_SIZE];

void setup() {
  Serial.begin(57600);

  connect_wifi();


  // ArduinoOTA.begin();

  // Serial.println("");
  // Serial.println("WiFi connected");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());


  // Serial.print("connecting to ");
  // Serial.println(host);

}

void loop() {
  if (!client.connected()) {
    connect();
    return;
  }
  Serial.println("listening: ");
  int size = listen();
  if (size > 0) {
    Serial.print("Received: ");
    Serial.println(size);
  }
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



void connect() {
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }

  handsake();
}

bool handsake() {
  Serial.println("starting handsake");
  send(hardware_id, strlen(hardware_id));
  //  String line = client.readStringUntil(0);
  int size = listen();

  Serial.print("Received reply to handsake ");
  Serial.println(size);
  Serial.print(" content ");
  Serial.println((char *)&buffer[0]);

  return true;
}



int send(const char* data, int size) {

  int packet_size = size + 2;
  byte buffer[packet_size];

  buffer[0] = (byte) ((size >> 8) & 0xFF);
  buffer[1] = (byte) (size & 0xFF);

  memcpy(&buffer[2], data, size);

  return client.write(&buffer[0], packet_size);
}


int listen() {
  int size;
  while((size = receive()) == -1) {
    delay(250);
  }
  return size;
}

int receive() {
  if (!client.available()) return -1;

  int size_buf[2];
  size_buf[0] = client.read();
  size_buf[1] = client.read();

  int size =  size_buf[1] | size_buf[0] << 8;
  if (size > BUFFER_SIZE) {
    Serial.println("ERROR: BUFFER OVER OVERFLOW");
    return -1;
  }
  clear_buffer();
  for (int i=0; i < size; i++) {
    buffer[i] = client.read();
  }

  return size;

  // coap_packet_t pkt;

  // if (coap_parse(&pkt, buffer, size) != 0) {
  //   Serial.print("Bad packet rc");
  //   return;
  // }

  // Serial.println("Good packet!");
  // return;
}

void clear_buffer() {
  memset(buffer, 0, BUFFER_SIZE);
}
