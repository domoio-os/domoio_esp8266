#ifndef DOMOIO_H
#define DOMOIO_H

#define FIRMWARE_VERSION "0.2.4"

#include "customizations.h"
void custom_setup();

#include "FS.h"


#ifdef SERIAL_LOG

#define PRINT(...) Serial.printf(__VA_ARGS__)
#define PRINTLN(str) Serial.println(str)

#else

#define PRINT(...) ((void)0)
#define PRINTLN(str) ((void)0)

#endif

#define NETWORK_TIMEOUT 15000


#include "message.h"

bool register_device(String name, String claim_code, String public_key);
bool is_reconnect_requested();
bool is_ota_requested();

void delete_credentials();
void reset();


#define OTA_REQUEST_URL_LENGTH 512
#define OTA_REQUEST_FINGERPRINT_LENGTH 128

class OTARequest {
 public:
  OTARequest(const char* url, int url_length) {
    if (url_length + 1 > OTA_REQUEST_URL_LENGTH) {
      return;
    }

    strncpy(&this->url[0], url, url_length);
    this->url[url_length] = 0;

    this->https = false;
  }

  OTARequest(const char* url, int url_length, const char* fingerprint, int fingerprint_length) {
    strncpy(&this->url[0], url, OTA_REQUEST_URL_LENGTH);
    strncpy(&this->fingerprint[0], fingerprint, OTA_REQUEST_FINGERPRINT_LENGTH);
    this->https = true;
  }

  void run();

 private:
  bool https;
  char url[OTA_REQUEST_URL_LENGTH];
  char fingerprint[OTA_REQUEST_FINGERPRINT_LENGTH];
};

void schedule_stock_ota_update();
void run_ota_update();
void schedule_ota_update(OTARequest *request);

/*
 * Serial
 */


void serial_loop();


/*
 * crypto
 */

int decrypt_hex(const char *src, char *out, int len);
int decrypt(const byte *src, byte *out, int len);
String get_public_key();

/*
 * config
 */


#define WIFI_CONFIG_FILE "/wifi"

class WifiConfig {
public:
  char ssid[32];
  char password[64];

  bool load() {
    SPIFFS.begin();
    File file = SPIFFS.open(WIFI_CONFIG_FILE, "r");
    file.readBytes(&this->ssid[0], 32);
    file.readBytes(&this->password[0], 64);
    file.close();
    SPIFFS.end();
    return true;
  }

  bool save() {
    SPIFFS.begin();
    File file = SPIFFS.open(WIFI_CONFIG_FILE, "w");
    file.write((uint8_t*) &ssid[0], 32);
    file.write((uint8_t*) &password[0], 64);
    file.close();
    SPIFFS.end();
    return true;
  }

  static bool is_configured() {
    SPIFFS.begin();
    bool exists = SPIFFS.exists(WIFI_CONFIG_FILE);
    SPIFFS.end();
    return exists;
  }

  static bool reset() {
    SPIFFS.begin();
    SPIFFS.remove(WIFI_CONFIG_FILE);
    SPIFFS.end();
    return true;
  }

  const char* get_ssid() { return &ssid[0]; }
  const char *get_password() { return &password[0]; }

  void set_ssid(const char *ssid) {
    strncpy(&this->ssid[0], ssid, 32);
  }
  void set_password(const char *password) {
    strncpy(&this->password[0], password, 64);
  }
};

class DomoioConfig {
 public:
  String api_url;

  String api_fingerprint;
  bool ssl_api;
  String host;
  int port;

  DomoioConfig() {
#ifdef DEV_ENV
    this->api_url = "http://10.254.0.200:4000";
    this->ssl_api = false;
    this->host = "10.254.0.200";
    this->port = 1234;
#else
    this->api_url = "https://app.domoio.com";
    this->ssl_api = true;
    this->api_fingerprint = "D7 79 01 81 F9 D4 53 17 D5 A9 5A EB 0D 78 C9 2B 7A E1 43 7F";
    this->host = "app.domoio.com";
    this->port = 5685;
#endif
  }
};

extern DomoioConfig domoio_config;

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
};

void setup_port(Port*);
void init_ports();
void reset_btn_callback();
Port * get_port(int port_id);
void set_port(int port_id, int value);


#endif //DOMOIO_H
