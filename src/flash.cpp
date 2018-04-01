#include "domoio_core.h"
#include "domoio.h"
#include "storage.h"
#include "reactduino.h"
#include <ESP8266httpUpdate.h>

bool ota_requested = false;
OTARequest *ota_request = NULL;
void schedule_ota_update(OTARequest *request) {
  ota_request = request;
  ota_requested = true;
}
bool is_ota_requested() { return ota_requested; }

void run_ota_update() { ota_request->run(); }

void OTARequest::run() {
  reactduino::dispatch(REACT_FLASHING);
  t_httpUpdate_return ret;

  if (this->https) {
    PRINT("Flash '%s' [%s]\n", &this->url[0], &this->fingerprint[0]);
    ret = ESPhttpUpdate.update(&this->url[0], "", &this->fingerprint[0]);
  }else {
    ret = ESPhttpUpdate.update(&this->url[0]);
  }

  delay(1000);
  switch(ret) {
  case HTTP_UPDATE_FAILED:
    //   ! Serial.println("HTTP_UPDATE_FAILD Error");
    PRINT("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    PRINTLN("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    PRINTLN("HTTP_UPDATE_OK");
    break;
  }

  reset();
}


#define OTA_UPDATE_FILENAME "/ota"

void try_ota_update_from_file() {
  SPIFFS.begin();

  if (!SPIFFS.exists(OTA_UPDATE_FILENAME)) {
    PRINTLN("OTA not found");
    SPIFFS.end();
    return;
  }

  PRINTLN("OTA FOUND!!");

  File f = SPIFFS.open(OTA_UPDATE_FILENAME, "r");
  String ssid = f.readStringUntil('\n');
  String wifi_password = f.readStringUntil('\n');
  String url = f.readStringUntil('\n');

  ssid.trim();
  wifi_password.trim();
  url.trim();

  OTARequest *request;
  if (strncmp(url.c_str(), "https", 5) == 0) {
    String ssl_fingerprint = f.readStringUntil('\n');
    ssl_fingerprint.trim();
    request = new OTARequest(url.c_str(), url.length(), ssl_fingerprint.c_str(), ssl_fingerprint.length());
  } else {
    request = new OTARequest(url.c_str(), url.length());
  }

  PRINT("SSID: %s\nPASSWORD: %s\n", ssid.c_str(), wifi_password.c_str());
  f.close();
  SPIFFS.remove(OTA_UPDATE_FILENAME);
  PRINTLN("Closing filesystem");
  SPIFFS.end();

  reactduino::dispatch(REACT_CONNECTING_WIFI);
  while (join_wifi_network(ssid.c_str(), wifi_password.c_str()) != true) {
    delay(2000);
  }

  request->run();
  reset();
}

void create_ota_update_file(const char *ssid, const char* wifi_password, const char* url, const char* ssl_fingerprint) {
  SPIFFS.begin();

  File f = SPIFFS.open(OTA_UPDATE_FILENAME, "w");
  f.println(ssid);
  f.println(wifi_password);
  f.println(url);

  if (strncmp(url, "https", 5) == 0) {
    f.println(ssl_fingerprint);
  }

  f.close();
  SPIFFS.end();
  PRINTLN("OTA File created");
}
