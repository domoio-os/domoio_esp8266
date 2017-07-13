#include <Arduino.h>
#include <Ticker.h>


#define STATE_LED 13

namespace led {

  int led_state = 0;

  void turn_on() {
    led_state = true;
    digitalWrite(STATE_LED, LOW);
  }
  void turn_off() {
    led_state = false;
    digitalWrite(STATE_LED, HIGH);
  }

  void blink() {
    if (led_state) turn_off();
    else turn_on();
  }
}
