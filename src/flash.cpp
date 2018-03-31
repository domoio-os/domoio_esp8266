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
    ret = ESPhttpUpdate.update(&this->url[0], "", &this->fingerprint[0]);
  }else {
    ret = ESPhttpUpdate.update(&this->url[0]);
  }

  delay(1000);
  switch(ret) {
  case HTTP_UPDATE_FAILED:
    //   ! Serial.println("HTTP_UPDATE_FAILD Error");
    PRINT("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
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
