#include <Arduino.h>
#include "domoio.h"

namespace led {

  int led_state = 0;

  void turn_on() {
    led_state = true;
    digitalWrite(INTERNAL_LED_PORT, LOW);
  }
  void turn_off() {
    led_state = false;
    digitalWrite(INTERNAL_LED_PORT, HIGH);
  }

  void blink() {
    if (led_state) turn_off();
    else turn_on();
  }
}
