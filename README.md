Domoio firmware for ESP8266 devices
===========================================

# Requirements
- Arduino IDE. Get the latest version from the [Arduino website](http://www.arduino.cc/en/main/software)
- The [ESP8266 core for Arduino](https://github.com/esp8266/Arduino)

# Setup
## Setup the Makefile
Create a valid Makefile using Makefile.sample or Makefile.nodemcuv2 as example

    cp Makefile.nodemcuv2 Makefile

## Flash the filesystem
Create certificates

     mkdir root_fs && cd root_fs
     openssl genrsa -out server.pem 1024
     openssl rsa -in server.pem -outform PEM -pubout -out server.pub.pem

     # Convert them to DER format
     openssl rsa -in server.pem -outform DER -pubout -out server.pub.der
     openssl rsa -in server.pem -outform DER -out server.der
     cd ..

Then, flash the new fs

     make flash_fs

## Create the files for customization
Create the customizations files and edit them.

    cp customizations.cpp.sample customizations.cpp
    cp customizations.h.sample customizations.h

## Flash the firmware

    make flash
