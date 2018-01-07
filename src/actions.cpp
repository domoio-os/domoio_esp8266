#include "domoio_core.h"
#include "domoio.h"
#include "cantcoap.h"
#include <MD5Builder.h>
#include "FS.h"

char *chunk_buffer;
int received;

void start_transfer(CoapPDU *msg) {
  uint8_t *payload = msg->getPayloadPointer();
  int length = buff2i(payload, 0);
  PRINT("Start transfer %i bytes\n", length);

  chunk_buffer = (char*) malloc((length + 1) * sizeof(char));
  received = 0;
}

void process_chunk(CoapPDU *msg) {
  uint8_t *payload = msg->getPayloadPointer();
  int index = buff2i(payload, 0);
  int start = CHUNK_LENGTH * index;

  int length = msg->getPayloadLength() - 2;
  length = min(length, CHUNK_LENGTH);
  received = received + length;

  memcpy(chunk_buffer + start, payload + 2, length);
}


void finish_transfer(CoapPDU *msg) {
  chunk_buffer[received] = 0;
  char *payload = (char *) msg->getPayloadPointer();

  MD5Builder md5_builder;
  md5_builder.begin();
  md5_builder.add(chunk_buffer);
  md5_builder.calculate();
  String md5 = md5_builder.toString();

  if (strcmp(payload, md5.c_str()) == 0) {
    PRINT("TRANFER OK\n");
  }

  free(chunk_buffer);
  received = 0;
}

void save_device(const char* device) {
  SPIFFS.begin();
  File f = SPIFFS.open("/device.json", "w");
  f.print(device);
  f.close();
  SPIFFS.end();
}
