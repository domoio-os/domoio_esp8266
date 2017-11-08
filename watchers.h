#ifndef WATCHERS_H
#define WATCHERS_H

#include "DHT.h"

class Watcher {
public:
  virtual void loop(long time) {}
  Watcher(long _duration) : duration(_duration) {}
protected:
  long last_exection;
  long duration;

  bool should_run(long time) {
    if (time < this->last_exection || (time - this->last_exection) > this->duration) {
      return true;
    } else {
      return false;
    }
  }

  void set_executed(long time) {
    this->last_exection = time;
  }
};



class DHTWatcher : public Watcher {
public:
 DHTWatcher(int _model, int _pin, int _port, long _duration) : Watcher(_duration), model(_model), pin(_pin), port(_port) {
    this->init();
    PRINT("Initializing duration: %d\n", this->duration);
  }

  void loop(long time);

private:
  const int model;
  const int pin;
  const int port;

  void init();
  bool getting_data;
  bool read_data();

  DHT *driver;
};


void setup_watcher(Watcher *watcher);
void watchers_loop();

#endif //WATCHERS_H
