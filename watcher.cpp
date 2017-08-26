#include "domoio.h"
#include "watchers.h"

#include "SimpleList.h"

SimpleList<Watcher*> watchers_list;

void setup_watcher(Watcher *watcher) {
  watchers_list.push_back(watcher);
}



void watchers_loop() {
  long time = millis();
  for (SimpleList<Watcher *>::iterator itr = watchers_list.begin(); itr != watchers_list.end(); ++itr) {
    Watcher *watcher = (Watcher *)*itr;
    watcher->loop(time);
  }
}
