//interuptions MOV & EAU + GPS
//messages LoRa
//lecture batterie
//sommeil + delestage
 
#include <ArduinoLowPower.h>
#include "MMA8451_IRQ.h"
#include "Adafruit_GPS.h"
#include "variables.h"

//----------------------declaration lora----------------------------------
#include "Lora_Module.h"  //library to convert data in uint8 in old canne_avertisseuse
#include "Conversion.h" //library use the LoRa network in old canne_avertisseuse

Lora_Module lora;
Conversion conv;
int32_t latitude = conv.float_int32("43.522589", 5);   //coordonnées par defauts
int32_t longitude = conv.float_int32("3.930729", 5);
//------------------------fin lora-----------------------------------

Adafruit_GPS GPS(&GPSSerial);
MMA8451_IRQ mma = MMA8451_IRQ();

//fonctions 
void startGPS(void);        //demarre de gps
void lectureGPS(void);      //recherche une position gps
void infoGPS(void);         //affiche certaines information du gps
void SENDALL(void);         //envoie un message complet
void SENDVIE(void);         //envoie un message vie
uint8_t lecture_batt(void); //lecture de la tension de la batterie

//fonctions exécutées lors d'une interuption (ISR)
void alarmEventEAU(void);
void alarmEventMOV(void);
void alarmEventCLK(void);

void setup() {

  pinMode(PinLEDProg, OUTPUT); //Pin 6
  pinMode(PinLEDEAU, OUTPUT);
  pinMode(PinLEDMOV, OUTPUT);
  pinMode(PinLEDSENDMSG, OUTPUT);
  pinMode(PinLEDLoRa, OUTPUT);
  pinMode(PinLEDMMA, OUTPUT);
  pinMode(PinLEDAlerteBat,OUTPUT);
  pinMode(GPS_EN, OUTPUT);
  digitalWrite(GPS_EN, HIGH);           //--------on démare le gps ici---------
    
//--------------------------------tests des LEDS------------------------------------------
  Serial.println("tests des LEDS");
  digitalWrite(PinLEDProg,HIGH);
  digitalWrite(PinLEDEAU,HIGH);
  digitalWrite(PinLEDMOV,HIGH);
  digitalWrite(PinLEDSENDMSG,HIGH);
  digitalWrite(PinLEDLoRa, HIGH);
  digitalWrite(PinLEDMMA, HIGH);
  digitalWrite(PinLEDAlerteBat, HIGH);
  delay(1000); //le temps que l'on voie des leds s'allumer
  
  digitalWrite(PinLEDEAU,LOW);
  digitalWrite(PinLEDMOV,LOW);
  digitalWrite(PinLEDSENDMSG,LOW);
  digitalWrite(PinLEDLoRa, LOW);
  digitalWrite(PinLEDMMA, LOW);
  digitalWrite(PinLEDAlerteBat, LOW);
  
  Serial.begin(9600);
 // while (!Serial) ;             //tant que on n'a pas ouvert le moniteur série le programme ne s'execute pas !!!
  // toute les messages sur le port série sont a enlever lors de l'utilisation finale
  Serial.println("- Serial start");

//-------------------------------------------------------------LoRa initialisation-------------------------------------------------------------------------------------
  Serial.println("- LoRa initialisation ..."); 
//  lora.Init();
//  lora.info_connect(); 
  digitalWrite(PinLEDLoRa, HIGH);
//************************************************************LoRa initialisation***************************************************************************************

//------------------------------------------------------------MMA initialisation----------------------------------------------------------------------------------------
  Serial.println("- MMA initialisation ..."); 
  if (! mma.begin()) {
    Serial.println("\t Couldnt start");
    digitalWrite(PinLEDMMA, LOW);
    while (1);
  }
  Serial.println("\t MMA8451 found!");
  digitalWrite(PinLEDMMA, HIGH);
 
  mma.enableInterrupt(); // Setup the I2C accelerometer
//***********************************************************MMA initialization*****************************************************************************************

//--------------------------GPS--------------------------
Serial.println("Start GPS");
  startGPS();
  
  time = millis(); 
  
Serial.println("Recuperation GPS");

do {
  GPS.read();
  GPS.parse(GPS.lastNMEA());
  Serial.println('0');
 } while (!GPS.fix && (millis() - time) <= GPStimeout); // il faut qu'on est une position gps ou timout de 3 mins
 
 digitalWrite(GPS_EN, LOW); //on etein le GPS ICI
 
 Serial.println("fix? " + String(GPS.fix));
 
  SENDALL();
  
//--------------------déclaration des interuptions ------------------------
  
  pinMode(PinEAU, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(PinEAU), alarmEventEAU, FALLING);  //mettre un condensateur anti rebont !!
  LowPower.attachInterruptWakeup(PinEAU, alarmEventEAU, FALLING);
  pinMode(PinMOV, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(PinMOV), alarmEventMOV, FALLING);
  LowPower.attachInterruptWakeup(PinMOV, alarmEventMOV, FALLING);
  pinMode(PinCLK, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(PinCLK), alarmEventCLK, FALLING);
  LowPower.attachInterruptWakeup(PinCLK, alarmEventCLK, FALLING);
  
  mma.clearInterrupt(); //au cas ou il y a eu detection de mouvement pendant la recherge cela l'anulle (meme s'il n'y a aucune conséquance sur l'envoie de nv msg)
  Serial.println("start V8");
  digitalWrite(PinLEDProg,LOW);
}

void loop()//-------------------------------------------------------------------------------------------------------------------------------
{
  digitalWrite(PinLEDProg,HIGH);

  if(alarmOccurredCLK) {  //interuption horloge
    
    Serial.println("CLK : " + String(NBalarmOccurredCLK) + "\t"); 
    alarmOccurredCLK=false; //on reset le drapeau
  
//  Serial.println(String(NBalarmOccurredCLK) +" == "+ String(NBCLK)); 
    if(NBalarmOccurredCLK == NBCLK) // tout les deux coup d'horloge après une interuption on reset les flags
    {
      NBCLK=0;  Serial.println("RESET"); 
  
      digitalWrite(PinLEDEAU,!digitalRead(PinEAU));

      alarmOccurredEAUP = false;
      alarmOccurredMOVP = false;
      alarmOccurredEAU = false;
      alarmOccurredMOV = false;
  
 // Serial.println(mma.readRegister(0x0C) && 0x04);
      mma.clearInterrupt();
      delay(10);
  
      bool flag = (mma.readRegister(0x0C) && 0x04);
      digitalWrite(PinLEDMOV,flag);
    } 
  
  
    if(NBalarmOccurredCLK >= NBsendvie)
    {
      SENDVIE();
      NBalarmOccurredCLK=0;
    }
  } // FIN CLK


  if (alarmOccurredEAU == true && alarmOccurredEAUP==false) 
  {
    Serial.println("INTERUPTION_EAU");
    digitalWrite(PinLEDEAU,HIGH);
  
    alerte=alerte_EAU;
    SENDALL();

    Serial.println(NBalarmOccurredCLK);
 
    if(NBCLK+NBreset > NBsendvie){ //si le nombre de coup d'horloge avant le réset est plus grand que le coup d'horloge total 
      NBCLK=NBalarmOccurredCLK+NBreset-NBsendvie;    //sachant que si c'est egal à 0 c'est imposible d'ou le +1
    } else {                        //SI NBalarmOccurredCLK+NBreset = 7 et NBsendvie = 5 :7-5 = 2
      NBCLK=NBalarmOccurredCLK+NBreset;        //si c'est pas le cas on ajoute le nombre avant reset
    }
    Serial.println(NBCLK);
    if(NBCLK>NBsendvie) NBCLK=1; //au cas ou cela ne fonctionne pas 
  
    alarmOccurredEAU = false;
    alarmOccurredEAUP = true;
  }

  else if (alarmOccurredMOV == true && alarmOccurredMOVP==false) {      //modification : else ici

    Serial.println("INTERUPTION_MOV");
    digitalWrite(PinLEDMOV,HIGH);
  
    alerte=alerte_MOV;
    mma.enableInterrupt();
    delay(10);
    SENDALL();

    Serial.println(NBalarmOccurredCLK);
  
    if(NBCLK+NBreset > NBsendvie){ //si le nombre de coup d'horloge avant le réset est plus grand que le coup d'horloge total 
      NBCLK=NBalarmOccurredCLK+NBreset-NBsendvie;    //sachant que si c'est egal à 0 c'est imposible d'ou le +1
    } else {                        //SI NBalarmOccurredCLK+NBreset = 7 et NBsendvie = 5 :7-5 = 2
      NBCLK=NBalarmOccurredCLK+NBreset;        //si c'est pas le cas on ajoute le nombre avant reset
    }
    Serial.println(NBCLK);
    if(NBCLK>NBsendvie) NBCLK=1; //au cas ou cela ne fonctionne pas 

    alarmOccurredMOV = false;
    alarmOccurredMOVP = true;
  }

delay(50);
digitalWrite(PinLEDProg,LOW);
LowPower.deepSleep();
}//fin du loop

//---------------------------------------------INTERUPTIONS-----------------------------------------------------------
void alarmEventEAU() {
  alarmOccurredEAU = true;
}

void alarmEventMOV() {
  alarmOccurredMOV = true;
}

void alarmEventCLK (void)
{
  NBalarmOccurredCLK++;
  alarmOccurredCLK = true;
}

//---------------------------------------------LORA------------------------------------------------------------
void SENDALL()
 {
  //--------------------------GPS----------------------
   if(alerte != 0 && !delestage){                 //quand on a une alerte=init, il n'y a pas besoins de refaire une recherge puisqu'on vient tout juste d'avoir un fix
    lectureGPS();                               // si on a plus assé de batterie on n'utilise plus le gps
   }
  //  infoGPS();
  batterie=lecture_batt(); //----------------recuperation de la tension batterie-------- 

  digitalWrite(PinLEDSENDMSG, HIGH);
  int errorsendA;
  
  Serial.print("\t \t \t Send alerte: " + String(alerte) +"\n");

  uint8_t buffer[9];
  buffer[0] = (uint8_t)(alerte << 5) + (uint8_t)(batterie & 0b11111);
  buffer[1] = (uint8_t)(longitude >> 24);
  buffer[2] = (uint8_t)(longitude >> 16);
  buffer[3] = (uint8_t)(longitude >> 8);
  buffer[4] = (uint8_t)longitude;
  buffer[5] = (uint8_t)(latitude >> 24);
  buffer[6] = (uint8_t)(latitude >> 16);
  buffer[7] = (uint8_t)(latitude >> 8);
  buffer[8] = (uint8_t)latitude;
  errorsendA = lora.send(buffer, 9);
//  Serial.println("Voici le code d'erreur_: " + String(errorsendA)); 
  
  delay(20);
  digitalWrite(PinLEDSENDMSG, LOW);  
}

void SENDVIE()
{
  Serial.println("Send VIE");
  
  batterie=lecture_batt(); //----------------recuperation de la tension batterie-------- 

  if(batterie < seuil_critique) delestage = true; 
  digitalWrite(PinLEDAlerteBat,delestage);

  if(batterie < seuil_critique && !alerte_BATP) { //si la batterie est vide et que l'on a pas déja envoyé le message on envoie un message avec le gps sinon on envoie le msg vie 
    alerte=alerte_BAT;
    SENDALL();
    alerte_BATP=true; //on a envoyé un message donc la prochaine fois on ne l'envera pas 
                      // si batterie vide alors on rentre dans un mode dégradé (GPS désactivé) == a coder
  } else
  {
    alerte=alerte_VIE;
    Serial.print("\t \t \t Send alerte: " + String(alerte) +"\n");
   
    digitalWrite(PinLEDSENDMSG, HIGH);
    int errorsendB;

    uint8_t buffer[1];  
    buffer[0] = (uint8_t)(alerte << 5) + (uint8_t)(batterie & 0b11111);
   
    errorsendB =lora.send(buffer, 1); 
//  Serial.println("Voici le code d'erreur_: " + String(errorsendB)); 
  
    delay(20);
    digitalWrite(PinLEDSENDMSG, LOW); 
  }
}


//---------------------------------------------GPS------------------------------------------------------------
void startGPS(){
  
  GPS.begin(9600);

  // Mode GGA et RMC
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
 
  Serial.println("Get version!");
  GPS.println(PMTK_Q_RELEASE);

  GPS.sendCommand(PGCMD_ANTENNA);
}

void infoGPS(void)
{
      Serial.print("Time: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
 /*   Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);*/
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    
    if (GPS.fix) {
    //  Serial.print("Location: ");
    //  Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
    //  Serial.print(", ");
     // Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
     //     Serial.print("Location (in degrees, works with Google Maps): ");
     //   Serial.print(GPS.latitudeDegrees, 4);
     //   Serial.print(", "); 
     //   Serial.println(GPS.longitudeDegrees, 4);

    //  Serial.print("Speed (knots): "); Serial.println(GPS.speed);
    //  Serial.print("Angle: "); Serial.println(GPS.angle);
    //  Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
}

void lectureGPS(void)
{
  digitalWrite(GPS_EN, HIGH);
  time = millis(); 
//  timer = millis();
     //si je recoit une nouvelle trame je sort de la boucle et que que son CRC est OK
  do {   //attente d'une nouvelle trame
     
  //   Serial.println("attente nouvelle trame");
    GPS.read(); 
    
    if (GPS.newNMEAreceived()) 
    {
    //  Serial.print("NEW");
      if(!GPS.parse(GPS.lastNMEA()))
      {
   //     Serial.println("\t CRC NO \t" +  String(nombre) + "\t" + String(GPS.fix) + "\t" + String(GPS.satellites) + "\t" + String(GPS.speed));
      } else
      {
      nombre++;
   //   Serial.println("\t CRC \t OK \t" + String(nombre) + "\t" + String(GPS.fix) + "\t" + String(GPS.satellites) + "\t" + String(GPS.speed));
//   j'attend de recevoir deux trames bonnes pour sortir de la boucle (pb rencontré : si je prend la premire recue c'est très probable que c'est celle que j'ai reçu la derniere fois
//  if(!GPS.fix) nombre=0;
    //  Serial.println("attente d'un fix");
      }
    }
  } while ((nombre<2 || !GPS.fix) && (millis() - time) <= GPStimeout); //2 minutes
  digitalWrite(GPS_EN, LOW);
  Serial.println("\t fix? " + String(GPS.fix) + "\t temps mis pour trouver le fix: " + String(millis() - time));

  nombre=0 ;
  longitude= conv.float_int32(GPS.longitudeDegrees, 5); //convertit le nombre flottant en nombre entier
  latitude= conv.float_int32(GPS.latitudeDegrees, 5);  
}

uint8_t lecture_batt (void)
{
 // float val=11.3;
  float val;
  analogReference(AR_DEFAULT);   //la tention de référence est 3.3v cela veut dire que 1023 = 3.3V
  val = analogRead(PinBatt);   //on lit la tension sur la broche PinBatt
 
  val=(val*3.3)/1023.0;
 // Serial.println("tension en volt : " + String(val));
  
  val=val/0.234;   //coef pont diviseur = 0.22 R1 = 326k, R2=100k
  val=val-10.0;   //precision de 0.16 (5/31) sur 5 volts au lieus des 0.45 V (14/31)

  if(val<0.0)
  {
    val=0.0;
    return val;
  } else {
     val=(val*31.0)/4.0;  //on code ici les 4 Volts sur 5 bits (que l'on va décoder plus tard grace a TTN)
     return val; 
  }  
}
