#ifndef REACTDUINO_H
#define REACTDUINO_H
#include <cstddef>

namespace reactduino {

  class View {

  public:
    virtual void initialize() {}
    virtual void loop() {}
  };


  class InputManager {
  public:
    virtual void initialize() {}
    virtual void loop() {}
  };

  class Controller {
  public:
    Controller() {
      this->initialize();
    }

    virtual void initialize() {}
    virtual void loop() {}
    virtual void dispatch(int action, void* data=NULL) {}
  };

  void push_controller(Controller *ctrl);
  void pop_controller();
  void loop();
  void dispatch(int action, void *data=NULL);
  Controller *current();

}

#endif //REACTDUINO_H
