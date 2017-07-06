#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>

#include <WiFiClientSecure.h>
#include <ArduinoOTA.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESP8266HTTPClient.h>

#include "cantcoap.h"
#include "domoio.h"
#include "wificonf.h"
#include "storage.h"
#include "reactduino.h"
#include "scenes.h"


void delete_credentials() {
  WifiConf::reset_config();
  reset();
}

void reset() {
  delay(5000);
  ESP.reset();
  delay(5000);
}

reactduino::StateContainer state;
HomeController home_controller(&state);

void setup() {
#ifdef SERIAL_LOG
  Serial.begin(115200);
#endif

  WiFi.persistent(true);
  Storage::begin();
  delay(1000);

  reactduino::push_controller(&home_controller);
  WifiConf::connect();

  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  reactduino::loop();

  if (!is_connected()) {
    connect();
    delay(250);
    return;
  }

  receive_messages();
}
