#include <Arduino.h>
#include "domoio.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FS.h"

ESP8266WebServer *server = NULL;;


void handleRoot() {
	server->send(200, "text/html", "<h1>You are connected</h1>");
}

void handleNotFound(){
  server->send(404, "text/html", "<h1>404 - Not Found</h1>");
}


bool wait_for_wifi() {
  double start = millis();
  while (WiFi.status() != WL_CONNECTED && start > (millis() - 10000)  ) {
    delay(500);
  }
  return WiFi.status() == WL_CONNECTED;
}
void reconnect(const char *ssid, const char *password) {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wait_for_wifi();
  ESP.reset();
  delay(1000);
}



String get_public_key(){

  SPIFFS.begin();
  File f = SPIFFS.open("/server.pub.der", "r");

  if (!f) {
    Serial.println("file open failed");
    return String("NULL");
  }
  int size = f.size();
  // byte buffer[size];
  // f.readBytes((char *)&buffer[0], size);

  int string_len = (size * 2) + 1;
  char buffer[string_len];



  for (int i=0; i < size; i++) {
    sprintf(&buffer[2 *i], "%02X", f.read());
  }
  buffer[string_len] = '\0';

  f.close();
  SPIFFS.end();
  return String(buffer);

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


void handle_submit() {
  String debug = "vars: \n";
  int count = server->args();
  for (int i=0; i < count; i++) {
    String name = server->argName(i);
    String value = server->arg(i);
    debug += name + " = " + value + "\n";
  }

  Serial.print(debug);

  String ssid = server->arg("ssid");
  String encrypted_pwd = server->arg("pwd");

  int pwd_len = encrypted_pwd.length();
  char pwd_buf[pwd_len];

  if (decrypt(encrypted_pwd.c_str(), &pwd_buf[0], pwd_len) < 0) {
    Serial.println("Error decrypt");
    server->send(401, "application/json", "{\"error\": \"Error decrypting\"}");
    return;
  }

  String buffer = "";
  buffer += "{\"ssid\":\"" + ssid + "\", \"status\":\"ok\"}";
  server->send(200, "application/json", buffer);
  reconnect(ssid.c_str(), &pwd_buf[0]);
}



void start_ap_mode() {
  IPAddress ip(192, 168, 5, 1);
  IPAddress gateway(192, 168, 5, 1);
  IPAddress netmask(255, 255, 255, 0);

  WiFi.config(ip, gateway, netmask);
  WiFi.softAP("domoio_device");
}


void run_config_server() {
  server = new ESP8266WebServer(80);
  server->on("/", handleRoot);
  server->on("/info", handle_info);
  server->on("/config", HTTP_POST, handle_submit);
  server->onNotFound(handleNotFound);
	server->begin();
  Serial.println("Server started");
  while(true) {
    server->handleClient();
  }
}

void connect_wifi() {
  WiFi.begin();
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

namespace WifiConf {
  void connect() {
    if (WiFi.SSID().length() == 0) {
      start_ap_mode();
      run_config_server();
    } else {
      connect_wifi();
    }
  }
  void reset_config() {
    WiFi.disconnect(true);
  }


}
