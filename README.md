Domoio firmware for ESP8266
===========================================
Domoio is a framework for home automation

# Create a new project
If you want to use domoio in your project, you'll need to build this library or download a precompiled binary. Check out the [releases page](https://github.com/eloy/domoio_esp8266/releases) to download the latest version, or read below the compile instructions.
## Requirements
- Arduino IDE. Get the latest version from the [Arduino website](http://www.arduino.cc/en/main/software)
- The [ESP8266 core for Arduino](https://github.com/esp8266/Arduino)
- makeEspArduino.mk Get it from its [repository](https://github.com/plerup/makeEspArduino)


The project layout should looks like this:

    sample_project
    |-- domoio.h
    |-- libdomoio.a
    |-- makeEspArduino.mk
    |-- Makefile
    |-- sample_project.ino

## Setup the Makefile
Create a valid Makefile. Ckeck out the dir examples/makefiles for examples. Visit [https://github.com/plerup/makeEspArduino](https://github.com/plerup/makeEspArduino) for more information about the parameters.

## Upload certificates
Each domoio device require unique pair of rsa keys. You have to generte and flash these keys just one time per device.


     mkdir root_fs && cd root_fs
     openssl genrsa -out server.pem 1024
     openssl rsa -in server.pem -outform PEM -pubout -out server.pub.pem

     # Convert them to DER format
     openssl rsa -in server.pem -outform DER -pubout -out server.pub.der
     openssl rsa -in server.pem -outform DER -out server.der
     rm server.pem server.pub.pem
     cd ..

     #Then, flash the new fs
     make flash_fs FS_DIR=./root_fs


## Create your scketch
To use the lib you must call it on setup and loop.

sample_project.ino

    #include <Arduino.h>
    #include "domoio.h"
    #include <FS.h>

    void setup() {
      // This macro is needed to make sure that all the dependencies are loaded
      __FIX_COMPILER_ISSUES;

      // Internal LED
      Port *state_led = new Port();
      state_led->id = 16;
      state_led->port_type = PORT_DIGITAL;
      state_led->io = PORT_OUTPUT;
      state_led->active_low = true;
      setup_port(state_led);
      domoio::set_led_port(state_led);

      // Reset BTN
      Port *reset_port = new Port();
      reset_port->id = 0;
      reset_port->port_type = PORT_DIGITAL;
      reset_port->io = PORT_INPUT;
      setup_port(reset_port);
      domoio::set_reset_port(reset_port);

      // Relay 1
      Port *relay_1 = new Port();
      relay_1->id = D1; //
      relay_1->port_type = PORT_DIGITAL;
      relay_1->io = PORT_OUTPUT;
      relay_1->active_low = true;
      setup_port(relay_1);

      domoio::setup();
    }

    void loop() {
      domoio::loop();
    }



## Flash the firmware

    make flash




# Build instructions
## Requierements
- Arduino IDE. Get the latest version from the [Arduino website](http://www.arduino.cc/en/main/software)
- The ESP8266 core for Arduino [https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino)
- axTLS port for ESP8266 [https://github.com/igrr/axtls-8266](https://github.com/igrr/axtls-8266)

This is the recomenended layout

    .
    |-- axtls-8266
    |-- esp8266
    |-- domoio_esp8266


## compile

     cd domoio_esp8266
     mkdir build
     cd build
     cmake -DESP_ROOT=$HOME/src/8266/esp8266 -DAXTLS_ROOT=$HOME/src/8266/axtls-8266/ ..
     make

If you have followed the recomened project layout, then you don't need to set ESP_ROOT or AXTLS_ROOT, you can just do:

     cd domoio_esp8266
     mkdir build
     cd build
     cmake ..
     make
