#include "domoio.h"
#include "Arduino.h"
#include "watchers.h"
#include <dht11.h>


dht11 DHT11;

void DHTWatcher::init() {
  // DHT11 = new idDHT11(this->pin, digitalPinToInterrupt(this->pin), dht11_wrapper);
  // if (!DHT11) {
  //   PRINT("Couldn't initialize the dht11 port");
  // }
}

void DHTWatcher::loop(long time) {
  // if (!DHT11) return;

  // if (getting_data) {
  //   if (DHT11->acquiring()) return;
  //   this->set_executed(time);
  //   this->getting_data = false;

  //   if (!this->read_data()) return;
  //   this->send_data();
  // }

  // if (should_run(time)) {
  //   DHT11->acquire();
  //   this->getting_data = true;
  // }


  if (should_run(time)) {
    int chk = DHT11.read(this->pin);
    if (chk != 0) {
      return;
    }

    FloatMessageValue temp(this->temp_port, DHT11.temperature);
    FloatMessageValue hum(this->hum_port, DHT11.humidity);

    MessageValue* changes[] = {&temp, &hum};
    send_ports_change(changes, 2);
  }

}


void DHTWatcher::send_data() {
  // FloatMessageValue temp(this->temp_port, DHT11->getCelsius());
  // FloatMessageValue hum(this->hum_port, DHT11->getHumidity());

  // MessageValue* changes[] = {&temp, &hum};
  // send_ports_change(changes, 2);
}

// bool DHTWatcher::read_data() {
//   int result = DHT11->getStatus();
//   switch (result) {
//   case IDDHTLIB_OK:
//     return true;
//   case IDDHTLIB_ERROR_CHECKSUM:
//     PRINT("Error\n\r\tChecksum error");
//     return false;
//   case IDDHTLIB_ERROR_ISR_TIMEOUT:
//     PRINT("Error\n\r\tISR Time out error");
//     return false;
//   case IDDHTLIB_ERROR_RESPONSE_TIMEOUT:
//     PRINT("Error\n\r\tResponse time out error");
//     return false;
//   case IDDHTLIB_ERROR_DATA_TIMEOUT:
//     PRINT("Error\n\r\tData time out error");
//     return false;
//   case IDDHTLIB_ERROR_ACQUIRING:
//     PRINT("Error\n\r\tAcquiring");
//     return false;
//   case IDDHTLIB_ERROR_DELTA:
//     PRINT("Error\n\r\tDelta time to small");
//     return false;
//   case IDDHTLIB_ERROR_NOTSTARTED:
//     PRINT("Error\n\r\tNot started");
//     return false;
//   default:
//     PRINT("Unknown error");
//     return false;
//   }

// }
