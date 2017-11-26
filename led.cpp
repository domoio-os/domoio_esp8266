#include "domoio.h"
#include "customizations.h"

namespace led {

  int led_state = 0;

  void turn_on() {
    led_state = true;
    set_port(INTERNAL_LED_PORT, 1);
  }
  void turn_off() {
    led_state = false;
    set_port(INTERNAL_LED_PORT, 0);
  }

  void blink() {
    if (led_state) turn_off();
    else turn_on();
  }
}
