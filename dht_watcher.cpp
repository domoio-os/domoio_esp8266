#include "domoio.h"
#include "Arduino.h"
#include "watchers.h"

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

      char out[50];
      snprintf(&out[0], 50, "{\"temp\":%.4f, \"hum\":%.4f}", t, h);
      send_json(this->port, &out[0]);
      this->set_executed(time);
    }
  }

}
