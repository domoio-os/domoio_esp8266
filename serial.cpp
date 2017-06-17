#include <stdio.h>
#include <Arduino.h>
#include "FS.h"

#include "domoio.h"

#include "ssl/crypto_misc.h"
#include "crypto/crypto.h"



#define SERIAL_BUFFER_LENGH 65

int read_int() {
  byte buffer[2];

  int size = Serial.readBytes(&buffer[0], 2);
  return buff2i(&buffer[0], 0);
}

int init_rsa_context(RSA_CTX **ctx) {
  SPIFFS.begin();
  File f = SPIFFS.open("/server.der", "r");

  if (!f) {
    Serial.println("file open failed");
    return -1;
  }
  int size = f.size();
  byte buffer[size];
  f.readBytes((char *)&buffer[0], size);
  f.close();
  SPIFFS.end();
  return asn1_get_private_key(&buffer[0], size, ctx);
}


int convert(const char *hex_str, unsigned char *byte_array, int byte_array_max) {
  uint8_t n;

  for (int i = 0; i < byte_array_max; i++) {
    sscanf(hex_str + 2*i, "%02x", &n);
    byte_array[i] = n;
  }
  return 0;
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
    uint8_t buffer[size];



    int converted = convert(&read_buffer[0], &buffer[0], size);

    RNG_initialize();
    RSA_CTX *ctx = NULL;


    if (init_rsa_context(&ctx) == -1) {
      Serial.println("Error reading cert");
      return;
    }

    uint8_t dec_data2[128];


    if (RSA_decrypt(ctx, &buffer[0], dec_data2, sizeof(dec_data2), 1) < 0) {
      Serial.println("Error decrypt");
    } else {
      Serial.println((char*) &dec_data2);
    }

    RSA_free(ctx);
    RNG_terminate();

  }

}
