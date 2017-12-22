#import "reactduino.h"
#define QUEUE_SIZE 15
namespace reactduino {

  Controller *queue[QUEUE_SIZE];
  int current_index = -1;


  void push_controller(Controller *ctrl) {
    current_index++;
    queue[current_index] = ctrl;
    ctrl->initialize();
  }

  void pop_controller() {
    current_index--;
  }

  Controller *current() {
    return queue[current_index];
  }
  void loop() {
    Controller *c = queue[current_index];
    c->loop();
  }

  void dispatch(int action, void *data) {
    Controller *c = current();
    c->dispatch(action, data);
  }
}
