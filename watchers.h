#ifndef WATCHERS_H
#define WATCHERS_H

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
  DHTWatcher(int _pin, int _temp_port, int _hum_port, long _duration) : Watcher(_duration), pin(_pin), temp_port(_temp_port), hum_port(_hum_port) {
    this->init();
  }

  void loop(long time);

private:
  const int pin;
  const int temp_port;
  const int hum_port;

  void init();
  bool getting_data;
  bool read_data();
  void send_data();
};


void setup_watcher(Watcher *watcher);
void watchers_loop();

#endif //WATCHERS_H