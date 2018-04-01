#include <Arduino.h>
#include "domoio_core.h"
#include "domoio.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "FS.h"
#include "reactduino.h"

extern "C" {
#include "user_interface.h"
}

ESP8266WebServer *server = NULL;
DNSServer *dns_server = NULL;

void handleRoot() {
	server->send(200, "text/html", "<h1>You are connected</h1>");
}

void handleNotFound(){
  server->send(404, "text/html", "<h1>404 - Not Found</h1>");
}

void start_ap_mode() {
  IPAddress ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress netmask(255, 255, 255, 0);

  WiFi.softAPConfig(ip, gateway, netmask);
  String ssid = "DIO_" + String(ESP.getChipId());
  WiFi.softAP(ssid.c_str());

  delay(1000);


  // Start the dns server
  dns_server = new DNSServer();
  dns_server->setErrorReplyCode(DNSReplyCode::NoError);
  dns_server->start(53, "*", ip);
}

bool wait_for_wifi() {
  double start = millis();
  while (WiFi.status() != WL_CONNECTED && start > (millis() - 10000)  ) {
    reactduino::loop();
    delay(250);
  }
  return WiFi.status() == WL_CONNECTED;
}

bool join_wifi_network(const char *ssid, const char *password) {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  if (wait_for_wifi()) {
    wifi_station_dhcpc_start();
    delay(3000);
    PRINTLN("Connected to new AP");
    PRINT("IP address: ");
    PRINTLN(WiFi.localIP());

    return true;
  } else {
    PRINTLN("Error connecting to new AP");
    start_ap_mode();
    return false;
  }
}

void handle_info() {
  String buffer = "{";

  buffer += "\"public_key\": \"" + get_public_key() + "\", ";

  // Networks
  int n = WiFi.scanNetworks();
  buffer += "\"networks\": [";

  for (int i=0; i < n; i++) {
    buffer += "{\"ssid\": \"" + WiFi.SSID(i) + "\", \"rssi\": " + WiFi.RSSI(i)+ "}";
    if (i != n -1) buffer += ", ";
  }

  buffer += "]";
  buffer += "}";
  server->send(200, "application/json", buffer);
}


void handle_config() {
  String ssid = server->arg("ssid");
  String encrypted_pwd = server->arg("pwd");

  String claim_code = server->arg("claim_code");
  String api_ssl_fingerprint = server->arg("api_ssl_fingerprint");
  int pwd_len = encrypted_pwd.length();
  char pwd_buf[pwd_len];

  if (decrypt_hex(encrypted_pwd.c_str(), &pwd_buf[0], pwd_len) < 0) {
    PRINTLN("Error decrypt");
    server->send(401, "application/json", "{\"error\": \"Error decrypting\"}");
    return;
  }

  dns_server->stop();

  reactduino::dispatch(REACT_AP_SETUP_WIFI_CONFIG);


  String buffer = "";
  buffer += "{\"ssid\":\"" + ssid + "\", \"status\":\"ok\"}";
  server->send(200, "application/json", buffer);

  delay(1000);

  while (join_wifi_network(ssid.c_str(), &pwd_buf[0]) != true) {
    delay(500);
  }

  PRINTLN("Registering device");

  while(register_device(claim_code, get_public_key(), api_ssl_fingerprint) != true) {
    delay(500);
  }

  // Save wifi config
  WifiConfig wifi_config;
  wifi_config.set_ssid(ssid.c_str());
  wifi_config.set_password(&pwd_buf[0]);
  wifi_config.save();

  PRINTLN("Done");
  reset();

}

int base_64_decoded_length(const char *data, int input_length);
void base64_decode(const char *data, size_t input_length, unsigned char *decoded_data, size_t output_length);

void handle_flash() {
  String ssid = server->arg("ssid");
  String encrypted_pwd = server->arg("pwd");
  String url_encoded = server->arg("url");
  String ssl_fingerprint = server->arg("ssl_fingerprint");

  int pwd_len = encrypted_pwd.length();
  char pwd_buf[pwd_len];

  PRINTLN("HANDLE FLASH");
  if (decrypt_hex(encrypted_pwd.c_str(), &pwd_buf[0], pwd_len) < 0) {
    PRINTLN("Error decrypt");
    server->send(401, "application/json", "{\"error\": \"Error decrypting\"}");
    return;
  }

  PRINTLN("Testing WIFI connection");
  while (join_wifi_network(ssid.c_str(), &pwd_buf[0]) != true) {
    delay(500);
  }

  delay(1000);
  WiFi.disconnect();

  // Decode url
  int output_length = base_64_decoded_length(url_encoded.c_str(), url_encoded.length());
  char url_buff[output_length + 1];
  base64_decode(url_encoded.c_str(), url_encoded.length(),  (unsigned char*) &url_buff[0], output_length);
  url_buff[output_length] = 0;

  PRINT("URL (%d): %s.\n", output_length, &url_buff[0]);

  create_ota_update_file(ssid.c_str(), &pwd_buf[0], &url_buff[0], ssl_fingerprint.c_str());

  reset();

  // dns_server->stop();
  // server->close();
  // server->stop();
  // free(server);
  // free(dns_server);

  // reactduino::dispatch(REACT_AP_SETUP_WIFI_CONFIG);


  // OTARequest *request;

  // if (ssl_fingerprint == NULL) {
  //   PRINTLN("Flashing firmware via HTTP");
  //   request = new OTARequest(&url_buff[0], output_length);
  // } else {
  //   PRINTLN("Flashing firmware via HTTPS");
  //   request = new OTARequest(&url_buff[0], output_length, ssl_fingerprint.c_str(), ssl_fingerprint.length());
  // }
  // schedule_ota_update(request);
}


void run_config_server() {
  server = new ESP8266WebServer(80);
  server->on("/", handleRoot);
  server->on("/info", handle_info);
  server->on("/config", HTTP_POST, handle_config);
  server->on("/flash", HTTP_POST, handle_flash);
  server->onNotFound(handleNotFound);
	server->begin();
  PRINT("AP Server started - v%s\n", DOMOIO_VERSION);
  reactduino::dispatch(REACT_AP_SERVER);
  while(true) {
    dns_server->processNextRequest();
    server->handleClient();
  }
}

void connect_to_ap() {
  WifiConfig wifi_config;
  wifi_config.load();
  reactduino::dispatch(REACT_CONNECTING_WIFI);
  PRINTLN("Connecting WiFI");
  WiFi.begin(wifi_config.get_ssid(), wifi_config.get_password());
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    reactduino::loop();
    delay(500);
  }
}


void connect_wifi() {
  if (WifiConfig::is_configured()) {
    WiFi.mode(WIFI_STA);
    connect_to_ap();
  } else {
    start_ap_mode();
    run_config_server();
  }
}
