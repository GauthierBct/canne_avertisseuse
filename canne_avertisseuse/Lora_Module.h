#ifndef LORA_MODULE_H_INCLUDED
#define LORA_MODULE_H_INCLUDED
#include <MKRWAN.h>
#include "commissioning.h"
#include <stdlib.h>
#include <stdint.h>
using namespace std;

#define Size_Max 64

class Lora_Module {
  public:
    Lora_Module();
    void Init();
    void Init_OTAA();
    void Init_ABP();
    int send(uint8_t *data, int Size);
    bool receive(uint8_t *data, double temps, int Size);
    void info_connect();

  private:
    LoRaModem modem;
};

#endif
