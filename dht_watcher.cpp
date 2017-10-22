#include "domoio.h"
#include "Arduino.h"
#include "watchers.h"


// DHT dht(D4, DHT11);

void DHTWatcher::init() {
  this->driver = new DHT(this->pin, this->model);
  if (!this->driver) {
    PRINT("Couldn't initialize the DHT driver");
  }

  this->driver->begin();
}

void DHTWatcher::loop(long time) {

  if (should_run(time)) {

    float h = this->driver->readHumidity();
    float t = this->driver->readTemperature();

    if (!isnan(t) && !isnan(h)) {
      PRINT("Temp: %.2f, Hum: %.2f\n", t, h);

      FloatMessageValue temp(this->temp_port, t);
      FloatMessageValue hum(this->hum_port, h);

      MessageValue* changes[] = {&temp, &hum};
      send_ports_change(changes, 2);
      this->set_executed(time);
    }
  }

}
