#ifndef SCENES_H
#define SCENES_H

#include <Arduino.h>
#include "reactduino.h"
#include "domoio.h"
#include "storage.h"

#define STATE_BTN 2
#define RESET_TIMEOUT 5000
using namespace reactduino;

class HomeIM : public InputManager {
  bool pressed;
  double pressed_at;
  bool reset_mode;

 public:
  HomeIM(StateContainer *_state) : InputManager(_state) {}

  void initialize() {
    pinMode(STATE_BTN, INPUT_PULLUP);
    this->pressed = false;
    this->reset_mode = false;
  }


  void loop() {
    bool btn_state = !digitalRead(STATE_BTN);

    if (btn_state && this->pressed) {
      if (!this->reset_mode && this->pressed_at + RESET_TIMEOUT < millis()) {
        this->reset_mode = true;
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("Reset mode on");
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
    Serial.println("pressed");
    this->pressed = true;
    this->pressed_at = millis();
  }

  void release() {
    Serial.println("released");
    this->pressed = false;
    this->pressed_at = NULL;

    if (this->reset_mode) {
      delete_credentials();
    }
  }
};

class HomeView : public View {
  bool led_state;
public:
  HomeView(StateContainer *_state) : View(_state) {}
  void initialize() {
    this->led_state = false;
  }
  void loop() {

  }

private:
  void turn_on() {
    this->led_state = true;
  }
  void turn_off() {
    this->led_state = false;
  }
};

class HomeController : public Controller {
  HomeView view;
  HomeIM im;
public:
  HomeController(StateContainer *_state) : Controller(_state), view(_state), im(_state) {

  }
  void initialize() {
    this->im.initialize();
    this->view.initialize();
  }

  void loop() {
    this->im.loop();
    this->view.loop();
  }
};

#endif //SCENES_H
