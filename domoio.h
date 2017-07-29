#ifndef DOMOIO_H
#define DOMOIO_H


#include "customizations.h"

#include "FS.h"

// #define DEV_ENV
#define SERIAL_LOG



#ifdef SERIAL_LOG

#define PRINT(__VA_ARGS__) Serial.printf(__VA_ARGS__)
#define PRINTLN(str) Serial.println(str)

#else

#define PRINT(str) ((void)0)
#define PRINTLN(str) ((void)0)

#endif



void delete_credentials();
void reset();

/*
 * messages
 */

enum ActionType {
  ACTION_LOG
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
  int send();
};

bool is_connected();
void connect();
void receive_messages();
int send(const void* data, int size);
bool register_device(String claim_code, String public_key);



void remote_log(const char* msg);
/*
 * Tools
 */


int buff2i(byte *buf, int offset);


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
    Serial.printf("SSID: %s PWD: %s\n", get_ssid(), get_password());
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

#endif //DOMOIO_H
