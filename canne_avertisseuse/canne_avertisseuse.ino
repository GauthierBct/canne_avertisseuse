//interuptions + GPS
//messages LoRa
//lecture batterie + alerte batterie GPS
 
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
void startGPS(void);
void lectureGPS(void);
void infoGPS(void);
void SENDALL(void);
void SENDVIE(void);
uint8_t lecture_batt(void);

//fonctions exécutées lors d'une interuption
void alarmEventEAU(void);
void alarmEventMOV(void);
void alarmEventCLK(void);

//en cour de dévelopement:
bool RECEPTION(void);

void setup() {

  pinMode(LED_BUILTIN, OUTPUT); //Pin 6
  pinMode(PinLEDEAU, OUTPUT);
  pinMode(PinLEDMOV, OUTPUT);
  pinMode(PinLEDSENDMSG, OUTPUT);
  pinMode(PinLEDLoRa, OUTPUT);
  pinMode(PinLEDMMA, OUTPUT);
  pinMode(GPS_EN, OUTPUT);
  digitalWrite(GPS_EN, HIGH);           //--------on démare le gps ici---------
    
//--------------------------------tests des LEDS------------------------------------------
  Serial.println("tests des LEDS");
  digitalWrite(LED_BUILTIN,HIGH);
  digitalWrite(PinLEDEAU,HIGH);
  digitalWrite(PinLEDMOV,HIGH);
  digitalWrite(PinLEDSENDMSG,HIGH);
  digitalWrite(PinLEDLoRa, HIGH);
  digitalWrite(PinLEDMMA, HIGH);

  delay(1000); //le temps que l'on voie des leds s'allumer
  
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(PinLEDEAU,LOW);
  digitalWrite(PinLEDMOV,LOW);
  digitalWrite(PinLEDSENDMSG,LOW);
  digitalWrite(PinLEDLoRa, LOW);
  digitalWrite(PinLEDMMA, LOW);
  
  Serial.begin(9600);
//  while (!Serial) ;             //tant que on n'a pas ouvert le moniteur série le programme ne s'execute pas !!!
  Serial.println("- Serial start");

//-------------------------------------------------------------LoRa initialisation-------------------------------------------------------------------------------------
  Serial.println("- LoRa initialisation ..."); 
  lora.Init();
  lora.info_connect(); 
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

//--------------------interuptions ------------------------
  
  pinMode(PinEAU, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinEAU), alarmEventEAU, FALLING);  //antit rebont !!
  pinMode(PinMOV, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinMOV), alarmEventMOV, FALLING);
  pinMode(PinCLK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinCLK), alarmEventCLK, FALLING);
  
//--------------------------GPS--------------------------
Serial.println("Start GPS");
  startGPS();
  
  GPStime = millis(); 
  timer = millis(); 
  
Serial.println("Recuperation GPS");

do {
  GPS.read();
  GPS.parse(GPS.lastNMEA());
 
  if((millis() - timer) >= 1000)
  {
    Serial.println("attente d'un fix");

    timer=millis();
  }
 } while (!GPS.fix && (millis() - GPStime) <= GPStimeout); // il faut qu'on est une position gps ou timout de 3 mins
 
 digitalWrite(GPS_EN, LOW); //on etein le GPS ICI
 //digitalWrite(PinLEDGPS,LOW);
 
 Serial.println("fix? " + String(GPS.fix));
 
  SENDALL();
  mma.clearInterrupt(); //au cas ou il y a eu detection de mouvement pendant la recherge cela l'anulle (meme s'il n'y a aucune conséquance sur l'envoie de nv msg)
  Serial.println("start V7.6");
}

void loop()
{

  if(alarmOccurredCLK) {Serial.println("CLK"); alarmOccurredCLK=false; };
  
  if (millis() - timer >= 5000)
{
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("Millis");

  digitalWrite(PinLEDEAU,!digitalRead(PinEAU));

  alarmOccurredEAUP = false;
  alarmOccurredMOVP = false;
  alarmOccurredEAU = false;
  alarmOccurredMOV = false;
  
 // Serial.println(mma.readRegister(0x0C) && 0x04);
  mma.clearInterrupt();
  delay(100);
  
  bool flag = (mma.readRegister(0x0C) && 0x04);
  digitalWrite(PinLEDMOV,flag);
  
  digitalWrite(LED_BUILTIN,LOW);
  tour++;
  timer=millis();
}


if (alarmOccurredEAU == true && alarmOccurredEAUP==false) 
{
  Serial.println("INTERUPTION_EAU");
  digitalWrite(PinLEDEAU,HIGH);
  
  alerte=alerte_EAU;
  
  SENDALL();
   alarmOccurredEAU = false;
   alarmOccurredEAUP = true;
}

else if (alarmOccurredMOV == true && alarmOccurredMOVP==false) {      //modification : else ici

  Serial.println("INTERUPTION_MOV");
  digitalWrite(PinLEDMOV,HIGH);
  
  alerte=alerte_MOV;
  mma.enableInterrupt();
  delay(100);
  SENDALL();
   alarmOccurredMOV = false;
   alarmOccurredMOVP = true;
}

if(tour >= 12) //12
{
  SENDVIE();
  tour=0;
}

//LowPower.deepSleep();
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
  alarmOccurredCLK = true;
}

//---------------------------------------------LORA------------------------------------------------------------
void SENDALL()
 {
  digitalWrite(PinLEDSENDMSG, HIGH);
  
  //--------------------------GPS----------------------
   if(alerte != 0){                 //quand on a une alerte=init, il n'y a pas besoins de refaire une recherge puisqu'on vient tout juste d'avoir un fix
    digitalWrite(GPS_EN, HIGH);
   
    delay(50);
    lectureGPS();
    digitalWrite(GPS_EN, LOW);
   }
    infoGPS();
  //-------------------fin GPS-------------------------
  
 batterie=lecture_batt(); //----------------recuperation de la tension batterie-------- 

  //-----------------------fin batt------------------------------
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
//if(errorsendA==0) { Serial.println("erreur envoie"); }
  
  delay(50);
  digitalWrite(PinLEDSENDMSG, LOW);
}

void SENDVIE()
{
  digitalWrite(PinLEDSENDMSG, HIGH);
  Serial.println("Send VIE");
  
  batterie=lecture_batt(); //----------------recuperation de la tension batterie-------- 

// if (alerte == alerte_EAU || alerte == alerte_MOV) {  //si une alérte a été précédament envoyé alors on renvoie une deuxieme foit cette alerte

//} else 
  if(batterie < seuil_critique && !alerte_BATP) { //si la batterie est vide et que l'on a pas déja envoyé le message on envoie un message avec le gps sinon on envoie le msg vie 
 //   Serial.println("batterie < seuil_critique || !alerte_BATP");
    alerte=alerte_BAT;
    SENDALL();
    alerte_BATP=true; //on a envoyé un message donc la prochaine fois on ne l'envera pas 
 //   Serial.println("alerte batp " + String(alerte_BATP));
                      // si batterie vide alors on rentre dans un mode dégradé (GPS désactivé) == a coder
  } else
  {
  alerte=alerte_VIE;
//}

   Serial.print("\t \t \t Send alerte: " + String(alerte) +"\n");

  int errorsendB;

  uint8_t buffer[1];  
   buffer[0] = (uint8_t)(alerte << 5) + (uint8_t)(batterie & 0b11111);
   
  errorsendB =lora.send(buffer, 1); 
//  Serial.println("Voici le code d'erreur_: " + String(errorsendB)); 
  
  delay(50);
  digitalWrite(PinLEDSENDMSG, LOW); 
  }
}

/*
while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  Serial.print("Received: ");
  for (unsigned int j = 0; j < i; j++) {
    Serial.print(rcv[j] >> 4, HEX);
    Serial.print(rcv[j] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
*/
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
       Serial.print(GPS.latitudeDegrees, 4);
        Serial.print(", "); 
        Serial.println(GPS.longitudeDegrees, 4);

    //  Serial.print("Speed (knots): "); Serial.println(GPS.speed);
    //  Serial.print("Angle: "); Serial.println(GPS.angle);
    //  Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
}

void lectureGPS(void)
{
  GPStime = millis(); 
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
      }
    }
  } while ((nombre<2 || !GPS.fix) && (millis() - GPStime) <= GPStimeout); //2 minutes

  Serial.println("\t fix? " + String(GPS.fix) + "\t temps mis pour trouver le fix: " + String(millis() - GPStime));

  nombre=0 ;
  longitude= conv.float_int32(GPS.longitudeDegrees, 5);  //ajout-------------------------------------------lora------------------------
  latitude= conv.float_int32(GPS.latitudeDegrees, 5);  //ajout-------------------------------------------lora------------------------
 // digitalWrite(PinLEDGPS,LOW);
}

uint8_t lecture_batt (void)
{
 // float val=11.3;
  float val;
  analogReference(AR_DEFAULT);   //la tention de référence est 3.3v cela veut dire que 1023 = 3.3V
  val = analogRead(PinBatt);   //on lit la tension sur la broche PinBatt
 
  val=(val*3.3)/1023.0;
  Serial.println("tension en volt : " + String(val));
  
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
