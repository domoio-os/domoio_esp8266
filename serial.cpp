#include <stdio.h>
#include <Arduino.h>

#include "domoio.h"


#define SERIAL_BUFFER_LENGH 65

int read_int() {
  byte buffer[2];

  int size = Serial.readBytes(&buffer[0], 2);
  return buff2i(&buffer[0], 0);
}




void serial_loop() {

  if (Serial.available() == 0) {
    return;
  }


  String cmd = Serial.readStringUntil('\n');

  if (cmd.equals("set_rsa")) {
    int read_size = read_int();
    char read_buffer[read_size + 1];
    int received = Serial.readBytes( &read_buffer[0], read_size);
    read_buffer[read_size] = '\0';

    Serial.print("Received: ");
    Serial.println(&read_buffer[0]);


    int size = read_size / 2;
    char buffer[size];


    if (decrypt(&read_buffer[0], &buffer[0], size) < 0) {
      Serial.println("Error decrypt");
    } else {
      Serial.println((char*) &buffer[0]);
    }

  }

}
