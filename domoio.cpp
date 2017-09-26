#include <Arduino.h>
#include <Ticker.h>
#include "domoio.h"
#include "cantcoap.h"
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include "storage.h"
#include "reactduino.h"

#define URI_BUFFER_LENGTH 25

#define BUFFER_SIZE 512

DomoioConfig domoio_config;

byte buffer[BUFFER_SIZE];
bool session_started = false;

WiFiClientSecure *client;


bool reconnect_requested = false;
bool is_reconnect_requested() { return reconnect_requested; }

int message_id_counter = 0;

double last_seen_at;

Ticker ping_ticker;

int next_message_id() {
  int id = message_id_counter++;
  return id;
}


void clear_buffer() {
  memset(buffer, 0, BUFFER_SIZE);
}

bool is_connected() {
  return client && client->connected();
}

int receive() {
  if (!client) return -1;

  if (!client->available()) return -1;
  last_seen_at = millis();

  int size_buf[2];
  size_buf[0] = client->read();
  size_buf[1] = client->read();

  int size = size_buf[1] | size_buf[0] << 8;
  if (size > BUFFER_SIZE) {
    PRINTLN("ERROR: OVERFLOW");
    return -1;
  }
  clear_buffer();
  for (int i=0; i < size; i++) {
    buffer[i] = client->read();
  }

  return size;
}


int block_until_receive() {
  int size = -1;
  double start = millis();
  while((size = receive()) == -1 && start + 5000 > millis()) {
    reactduino::loop();
  }
  return size;
}


const char *expected = "HELLO";
int expected_len = strlen(expected);

bool handsake() {
  int version_len = strlen(FIRMWARE_VERSION);
  int hello_len = 36 + version_len;
  char hello_buf[hello_len];
  if (Storage::get_device_id(&hello_buf[0], 37) == -1) {
    PRINTLN("Error reading device id");
    return false;
  }

  strncpy(&hello_buf[36], FIRMWARE_VERSION, version_len);

  PRINT("hello: %s", &hello_buf[0]);

  send(&hello_buf[0], hello_len);

  // Read the encryptd nounce
  int size = block_until_receive();


  byte nounce_clean[size];
  decrypt(&buffer[0], &nounce_clean[0], size);
  send(&nounce_clean, 40);

  // Read the encryptd nounce
  size = block_until_receive();


  if (size != expected_len || strncmp(expected, (char *) &buffer[0], size) != 0) {
    PRINTLN("BAD LOGIN");
    return false;
  }

  session_started = true;
  PRINTLN("Session started");
  last_seen_at = millis();
  return true;
}



int send(const void* data, int size) {
  if (!client) return -1;
  int packet_size = size + 2;
  byte buffer[packet_size];

  buffer[0] = (byte) ((size >> 8) & 0xFF);
  buffer[1] = (byte) (size & 0xFF);

  memcpy(&buffer[2], data, size);

  return client->write(&buffer[0], packet_size);
}

int send_confirmation(CoapPDU *msg) {
  CoapPDU reply;

	reply.setType(CoapPDU::COAP_ACKNOWLEDGEMENT);
	reply.setCode(msg->getCode());
	reply.setToken(msg->getTokenPointer(), msg->getTokenLength());
	reply.setMessageID(msg->getMessageID());
  return send(reply.getPDUPointer(), reply.getPDULength());
}


void ping() {
  // Check the timeout
  double current_time = millis();
  if ((current_time - last_seen_at) > NETWORK_TIMEOUT) {
    reconnect_requested = true;
    return;
  }


  CoapPDU ping_pk;
	ping_pk.setType(CoapPDU::COAP_CONFIRMABLE);
  ping_pk.setCode(CoapPDU::COAP_GET);
  ping_pk.setURI("/ping");
  ping_pk.setMessageID(next_message_id());
  send(ping_pk.getPDUPointer(), ping_pk.getPDULength());
}

void start_ping() {
  ping_ticker.attach(10, ping);
}

void stop_ping() {
  ping_ticker.detach();
}


void connect() {
  reactduino::dispatch(REACT_CONNECTING_DOMOIO);
  PRINTLN("connecting to Domoio");
  client = new WiFiClientSecure();
  if (!client->connect(domoio_config.host.c_str(), domoio_config.port)) {
    PRINTLN("connection failed");
    return;
  }

  if (handsake()) {
    reactduino::dispatch(REACT_CONNECTED);
    start_ping();
  }
}

void disconnect() {
  if (!client) return;
  stop_ping();
  reconnect_requested = false;
  client->stop();
  delete(client);
  client = NULL;
}


void process_message(CoapPDU *msg) {
  char uri_buf[URI_BUFFER_LENGTH];
  int uri_size;
  msg->getURI(&uri_buf[0], URI_BUFFER_LENGTH, &uri_size);

  if (strncmp(uri_buf, "/set_ports", 11) == 0) {
    int payload_length = msg->getPayloadLength();
    uint8_t *payload = msg->getPayloadPointer();

    int ports_length = payload_length / MESSAGE_VALUE_LENGTH;
    for (int i=0; i < ports_length; i++) {
      int offset = i * MESSAGE_VALUE_LENGTH;
      byte *buffer = payload + offset;
      int port_id = buff2i(buffer, 0);

      if (*(buffer + 2) == 0) {
        int value = buff32_to_int(buffer + 3);
        set_port(port_id, value);
      }
    }

    send_confirmation(msg);
  }

  else if (strncmp(uri_buf, "/set", 4) == 0) {
    int payload_length = msg->getPayloadLength();
    uint8_t *payload = msg->getPayloadPointer();

    int port_id = buff2i(payload, 0);
    if (*(payload + 2) == 0) {
      int value = buff32_to_int(payload + 3);
      set_port(port_id, value);
    }

    send_confirmation(msg);
  }
  else if (strncmp(uri_buf, "/flash_url", 10) == 0) {
    PRINTLN("Flashing from url");
    int payload_length = msg->getPayloadLength();
    uint8_t *payload = msg->getPayloadPointer();

    // Check the first byte to know whether or not the url includes a fingerprint
    bool https = payload[0] == 1;
    OTARequest *request;
    if (false) {
      // TODO: Implement HTTPS
    } else {
      request = new OTARequest((char*) payload + 1, payload_length - 1);
    }

    schedule_ota_update(request);
    send_confirmation(msg);
  }

  else if (strncmp(uri_buf, "/flash", 6) == 0) {
    PRINTLN("Flashing device");
    send_confirmation(msg);
    schedule_stock_ota_update();
  }
  else if (strncmp(uri_buf, "/ping", 5) == 0) {
    send_confirmation(msg);
  }

}



void receive_messages() {
  int size = receive();

  if (size <= 0) {
    return;
  }

  CoapPDU coap_msg = CoapPDU(buffer, size);

  if (!coap_msg.validate()) {
    PRINT("Bad packet rc");
    return;
  }

  process_message(&coap_msg);
}


bool register_device(String name, String claim_code, String public_key) {
  HTTPClient http;
  String url = domoio_config.api_url + "/api/register_device";
  bool success = false;
  PRINTLN(url);
  bool begin_success;
  if (domoio_config.ssl_api) {
    begin_success = http.begin(url, domoio_config.api_fingerprint);
  } else {
    begin_success = http.begin(url);
  }
  if (!begin_success ) {
    PRINT("Error connecting to domoio ");
    return false;
  }

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String post_data("");
  post_data += "name=" + name +
    "&claim_code=" + claim_code +
    "&device[type]=esp8266" +
    "&device[public_key]=" + public_key +
    "&device[hardware_id]=" + String(ESP.getChipId());

#ifdef PRODUCT_VERSION
  post_data += "&device[product_version]=" + String(PRODUCT_VERSION);
#endif
#ifdef FIRMWARE_VERSION
  post_data += "&device[firmware_version]=" + String(FIRMWARE_VERSION);
#endif

  int resp_code = http.POST(post_data);
  if (resp_code > 0) {
    String resp = http.getString();
    char received_device_id[37];
    // PRINT("Received: %s\n", resp.c_str());

    if(resp.startsWith("{\"errors\":")) {
      PRINT("Error registering device");
      goto error;
    }

    if (sscanf(resp.c_str(), "{\"device_id\":\"%36s\"}", &received_device_id[0]) > 0 ) {
      PRINT("Received device_id: %s", &received_device_id[0]);

      char device_id[37];
      if (Storage::get_device_id(&device_id[0], 37) == -1) {
        PRINTLN("Error reading device id");
        goto error;
      }

      success = true;

      if (strncmp(&device_id[0], &received_device_id[0], 36) != 0) {
        // Save the device_id
        Storage::set_device_id(&received_device_id[0]);
      }
    }

  } else {
    PRINT("ERROR: %s", http.errorToString(resp_code).c_str());
  }

 error:
  http.end();
  return success;
}
