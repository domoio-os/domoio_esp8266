#ifndef ACTIONS_H
#define ACTIONS_H

class CoapPDU;

void start_transfer(CoapPDU *msg);
void process_chunk(CoapPDU *msg);
void finish_transfer(CoapPDU *msg);


#endif //ACTIONS_H
