#ifndef SCENES_H
#define SCENES_H


#include <Arduino.h>
#include <Ticker.h>
#include "reactduino.h"
#include "domoio.h"
#include "storage.h"

#define STATE_BTN 0
#define STATE_LED 13

#define RESET_TIMEOUT 5000
using namespace reactduino;




namespace led {
  void blink();
  void turn_on();
  void turn_off();
}

Ticker flipper;

class HomeIM : public InputManager {
  bool pressed;
  double pressed_at;
  bool reset_mode;

 public:
  HomeIM() : InputManager() {}

  void initialize() {
    this->pressed = false;
    this->reset_mode = false;
  }


  void loop() {
    bool btn_state = !digitalRead(STATE_BTN);

    if (btn_state && this->pressed) {
      if (!this->reset_mode && this->pressed_at + RESET_TIMEOUT < millis()) {
        this->reset_mode = true;
        digitalWrite(LED_BUILTIN, LOW);
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

 private:
  void press() {
    PRINTLN("pressed");
    this->pressed = true;
    remote_log("BTN pressed");
    this->pressed_at = millis();
  }

  void release() {
    PRINTLN("released");
    remote_log("BTN released");
    this->pressed = false;
    this->pressed_at = NULL;

    if (this->reset_mode) {
      delete_credentials();
    }
  }
};



class HomeController : public Controller {
  HomeIM im;
  int current_action;

public:
  HomeController() : Controller() {
    this->current_action = -1;
  }

  ~HomeController() {
  }
  void initialize() {
    pinMode(STATE_BTN, INPUT_PULLUP);
    pinMode(STATE_LED, OUTPUT);

    this->im.initialize();
  }

  void loop() {
    this->im.loop();
  }

  void dispatch(int action, void *data) {
    if (action == this->current_action) return;

    switch (action) {
    case REACT_CONNECTING_WIFI:
      flipper.detach();
      flipper.attach(0.3, led::blink);
      return;

    case REACT_CONNECTING_DOMOIO:
    case REACT_FLASHING:
    case REACT_AP_RESET_WIFI_CONFIG:
      flipper.detach();
      flipper.attach(0.1, led::blink);
      return;
    case REACT_CONNECTED:
      flipper.detach();
      led::turn_on();
      return;
    case REACT_AP_SERVER:
      flipper.detach();
      flipper.attach(1, led::blink);
      return;

    }
  }
};

#endif //SCENES_H
