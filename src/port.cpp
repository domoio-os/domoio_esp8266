#include "domoio.h"
#include <Arduino.h>
#include "SimpleList.h"

SimpleList<Port*> ports_list;

void setup_port(Port *port) {
  ports_list.push_back(port);
}


Port * get_port(int port_id) {
  for (SimpleList<Port *>::iterator itr = ports_list.begin(); itr != ports_list.end(); ++itr) {
    Port *port = (Port *)*itr;
    if (port->id == port_id) return port;
  }
  return NULL;
}


void Port::set_value(int value) {
  if (this->active_low) {
    value = value == 0 ? 1 : 0;
  }

  this->value = value;
  if (this->port_type == PORT_DIGITAL) {
    if (value) {
      digitalWrite(this->id, HIGH);
    } else {
      digitalWrite(this->id, LOW);
    }
  }
}


void init_ports() {
  for (SimpleList<Port *>::iterator itr = ports_list.begin(); itr != ports_list.end(); ++itr) {
    Port *port = (Port *)*itr;
    if (port->io == PORT_OUTPUT) {
      port->set_value(0);
      pinMode(port->id, OUTPUT);
    } else {
      pinMode(port->id, INPUT);
    }
    PRINT("Setting up port %d", port->id);
  }

}
