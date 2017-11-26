#include "domoio.h"
#include "Arduino.h"
#include "watchers.h"
#include "HLW8012.h"



#define CURRENT_MODE                    HIGH

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 5 * 470000 ) // Real: 2280k
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000 ) // Real 1.009k

HLW8012 hlw8012;


void ICACHE_RAM_ATTR hlw8012_cf1_interrupt() {
  hlw8012.cf1_interrupt();
}
void ICACHE_RAM_ATTR hlw8012_cf_interrupt() {
  hlw8012.cf_interrupt();
}


void setInterrupts() {
}


void HLW8012Watcher::init() {
  hlw8012.begin(this->cf_pin, this->cf1_pin, this->sel_pin, CURRENT_MODE, true);
  hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);
  attachInterrupt(this->cf1_pin, hlw8012_cf1_interrupt, CHANGE);
  attachInterrupt(this->cf_pin, hlw8012_cf_interrupt, CHANGE);
}

void HLW8012Watcher::loop(long time) {

  if (should_run(time)) {

    unsigned int voltage = hlw8012.getVoltage();
    unsigned int active_power = hlw8012.getActivePower();
    unsigned int apparent_power = hlw8012.getApparentPower();
    int power_factor = (int) (100 * hlw8012.getPowerFactor());
    unsigned long energy = hlw8012.getEnergy();
    double current = hlw8012.getCurrent();

    // PRINT("Temp: %.2f, Hum: %.2f\n", t, h);

    char out[250];
    snprintf(&out[0], 250, "{\"volt\": %d, \"act_power\": %d, \"app_power\": %d, \"power_factor\": %d, \"current\": %.4f, \"energy\": %lu}", voltage, active_power, apparent_power, power_factor, current);
    send_json(this->port, &out[0]);
    this->set_executed(time);

  }

}
