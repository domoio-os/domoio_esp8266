#include <Arduino.h>
#include "domoio.h"

#include "FS.h"
#include "ssl/crypto_misc.h"
#include "crypto/crypto.h"


int init_rsa_context(RSA_CTX **ctx) {
  SPIFFS.begin();
  File f = SPIFFS.open("/server.der", "r");

  if (!f) {
    PRINTLN("file open failed");
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


int decrypt_hex(const char *src, char*out, int len) {
  int bin_len = len / 2;
  uint8_t encrypted[bin_len];
  int converted = convert(src, &encrypted[0], bin_len);

  RSA_CTX *ctx = NULL;


  if (init_rsa_context(&ctx) == -1) {
    PRINTLN("Error reading cert");
    return -1;
  }

  int ret_value = RSA_decrypt(ctx, &encrypted[0], (uint8_t *) out, len, 1);
  PRINTLN(&out[0]);
  RSA_free(ctx);
  return 1;
}


int decrypt(const byte *src, byte *out, int len) {
  RSA_CTX *ctx = NULL;

  if (init_rsa_context(&ctx) == -1) {
    PRINTLN("Error reading cert");
    return -1;
  }

  int ret_value = RSA_decrypt(ctx, src, out, len, 1);
  RSA_free(ctx);
  return 1;
}
