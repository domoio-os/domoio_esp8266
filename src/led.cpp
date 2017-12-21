#include "domoio.h"
#include "domoio_core.h"
#include "scenes.h"

Port *led_port = NULL;
Port *reset_port = NULL;
Ticker flipper;

using namespace reactduino;

namespace domoio {
  void set_led_port(Port *port) {
    led_port = port;
  }

  void set_reset_port(Port* port) {
    reset_port = port;
  }

}

namespace led {

  int led_state = 0;

  void turn_on() {
    led_state = true;
    if (led_port) led_port->set_value(1);
  }
  void turn_off() {
    led_state = false;
    if (led_port) led_port->set_value(0);
  }

  void blink() {
    if (led_state) turn_off();
    else turn_on();
  }
}


void HomeIM::loop() {
  bool btn_state = !digitalRead(reset_port->id);

  if (btn_state && this->pressed) {
    if (!this->reset_mode && this->pressed_at + RESET_TIMEOUT < millis()) {
      this->reset_mode = true;
      PRINTLN("Reset mode on");
      reactduino::dispatch(REACT_AP_RESET_WIFI_CONFIG);
    }
    return;
  }

  if (btn_state) {
    this->press();
    return;
  }

  if (!btn_state && this->pressed) {
    this->release();
    return;
  }

}
