#ifndef REACTDUINO_H
#define REACTDUINO_H

namespace reactduino {
  class StateContainer {

  };

  class View {
    StateContainer *state;
  public:
    View(StateContainer *_state) : state(_state) {
      this->initialize();
    }
    virtual void initialize() {}
    virtual void loop() {}
  };


  class InputManager {
    StateContainer *state;
  public:
    InputManager(StateContainer *_state) : state(_state) {
      this->initialize();
    }
    virtual void initialize() {}
    virtual void loop() {}
  };

  class Controller {
    StateContainer *state;
  public:
    Controller(StateContainer *_state) : state(_state) {
      this->initialize();
    }

    virtual void initialize() {}
    virtual void loop() {}

  };

  void push_controller(Controller *ctrl);
  void pop_controller();
  void loop();
  Controller *current();

}

#endif //REACTDUINO_H
