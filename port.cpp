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

void set_port(int port_id, int value) {
  Port * port = get_port(port_id);
  if (port == NULL) {
    PRINT("Invalid port requested: %d", port_id);
    return;
  }
  port->value = value;
  if (port->port_type == PORT_DIGITAL) {
    if (value) {
      digitalWrite(port->id, HIGH);
    } else {
      digitalWrite(port->id, LOW);
    }
  }
}

void init_ports() {
  for (SimpleList<Port *>::iterator itr = ports_list.begin(); itr != ports_list.end(); ++itr) {
    Port *port = (Port *)*itr;
    if (port->io == PORT_OUTPUT) {
      pinMode(port->id, OUTPUT);
    } else {
      pinMode(port->id, INPUT);
    }
    PRINT("Setting up port %d", port->id);
  }

}
