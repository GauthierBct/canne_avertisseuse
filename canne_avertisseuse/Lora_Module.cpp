#include "Lora_Module.h"

Lora_Module::Lora_Module() {
}

void Lora_Module::Init() {

  if (!modem.begin(EU868)) {
    while (1) {}
  }
  delay(1000);
  modem.dutyCycle(DutyCycle);
  modem.publicNetwork(PublicNetwork);
  modem.dataRate(DataRate);
  modem.setADR(ADR);
  Serial.print("deviceEUI "); Serial.println(modem.deviceEUI());
  if (OVER_THE_AIR_ACTIVATE) Lora_Module::Init_OTAA();
  else  Lora_Module::Init_ABP();
  modem.sleep();
}

void Lora_Module::Init_OTAA() {
  int connected = modem.joinOTAA(APP_EUI, APP_KEY);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    NVIC_SystemReset();
  }
  modem.minPollInterval(60);
}
void Lora_Module::Init_ABP() {
  int connected = modem.joinABP(DEVADDR, NWKSKEY, APPSKEY);
  if (!connected) {
    Serial.println("error:Joing failed");
    while (1) {}
  }
  modem.minPollInterval(60);
}
void Lora_Module::info_connect() {
  Serial.println("connection Lora:\n------------------------");
  Serial.print("Data Rate="); Serial.println(modem.getDataRate());
  Serial.print("ADR="); Serial.println(modem.getADR());
  if (OVER_THE_AIR_ACTIVATE) {
    Serial.println("----OTAA----");
    Serial.print("appEUI "); Serial.println(APP_EUI);
    Serial.print("appKey "); Serial.println(APP_KEY);
  }
  Serial.println("------------------------------------------");
  Serial.print("devAddr "); Serial.println(modem.getDevAddr());
  Serial.print("nwkSkey "); Serial.println(modem.getNwkSKey());
  Serial.print("appSkey "); Serial.println(modem.getAppSKey());
  Serial.println("------------------------------------------");

}



int Lora_Module::send(uint8_t *buffer, int len) {
  int err;
  int i;
  i=5;
  modem.dataRate(0);        //force le datarate à SF12  125
  modem.beginPacket();
  modem.write(buffer, len);
  err = modem.endPacket(true);
  modem.sleep();
  Serial.println("Voici le code d'erreur : " + String(err));
  if (err > 0) {
    Serial.println("message correctly send !");
    return err;
  }
  else {
    while (err<0 && i>0){
      modem.beginPacket();
      modem.write(buffer, len);
      err = modem.endPacket(true);
      i--;
      Serial.println("erreur : nouvel essais dans 2 secondes plus que "+ String(i));
      delay(2000);
    }
    return err;                                     //retourne un 1 si c'est positif ou un 0 si c'est négatif
  }
}

bool Lora_Module::receive(uint8_t *data, double temp, int Size) {

  delay(temp);
  if (!modem.available()) {

    return 0;
  }
  int i = 0;
  while (modem.available()) {
    data[i++] = (uint16_t)modem.read();
  }
  return 1;
  modem.sleep();

}
