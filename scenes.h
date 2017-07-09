#ifndef SCENES_H
#define SCENES_H

#include <Arduino.h>
#include "reactduino.h"
#include "domoio.h"
#include "storage.h"

#define STATE_BTN 0
#define STATE_LED 13

#define RESET_TIMEOUT 5000
using namespace reactduino;

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
        // PRINTLN("Reset mode on");
        remote_log("Reset mode on");
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


class Blink : public View {
  bool led_state;
  unsigned long previous_millis;
  unsigned long current_millis;
  const long interval;

public:
  Blink(long _interval) : interval(_interval), View() {}
  Blink() : Blink(1000) {}

  void initialize() {
    this->previous_millis = 0;
    this->turn_on();
  }

  void loop() {
    current_millis = millis();

    if (current_millis - previous_millis >= interval) {
      previous_millis = current_millis;

      if (led_state) this->turn_off();
      else this->turn_on();
    }
  }

private:
  void turn_on() {
    this->led_state = true;
    digitalWrite(STATE_LED, LOW);
  }
  void turn_off() {
    this->led_state = false;
    digitalWrite(STATE_LED, HIGH);
  }
};


class LedOn : public View {
public:
  void initialize() {

    digitalWrite(STATE_LED, LOW);
  }
};



class HomeController : public Controller {
  View *view;
  HomeIM im;
public:
  HomeController() : Controller() {
    this->view = new Blink(500);
  }

  ~HomeController() {
    if (this->view != NULL) {
      delete(this->view);
    }
  }
  void initialize() {
    pinMode(STATE_BTN, INPUT_PULLUP);
    pinMode(STATE_LED, OUTPUT);

    this->im.initialize();
    this->view->initialize();
  }

  void loop() {
    this->im.loop();
    this->view->loop();
  }

  void setView(View *v) {
    if (this->view != NULL) {
      delete(this->view);
    }
    this->view = v;
    this->view->initialize();
  }

  void dispatch(int action, void *data) {

    switch (action) {
    case REACT_CONNECTED:
      this->setView(new LedOn());
      return;

    case REACT_CONNECTING_DOMOIO:
      this->setView(new Blink(250));
      return;

    }
  }
};

#endif //SCENES_H
