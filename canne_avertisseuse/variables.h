// fichier comprenant toutes les variables
#define GPSSerial Serial1

#define alerte_INIT 0
#define alerte_VIE  1 
#define alerte_EAU  2 
#define alerte_MOV  3
#define alerte_BAT  4

#define GPStimeout 120000 //120000 = 2 minutes
#define NBsendvie 5 //10sec*5 = 50 sec    nombre de coup d'horloge avant le message de vie
#define NBreset 2 //10sec*2 = 20 sec      la led reset alumé pandant 20 seconde après l'envoie du message nombre de coup d'horloge avant le reset

#define deflatitude  "0.000000" //coordonnées par defauts
#define deflongitude "0.000000"

//---------------------variables---------------------
//ISRs
volatile bool alarmOccurredEAU = false;
volatile bool alarmOccurredMOV = false;
volatile bool alarmOccurredCLK = false;
volatile uint8_t NBalarmOccurredCLK = 0;

uint32_t time = 0;         // pour le GPS
uint32_t timer = 0;         // juste pour la boucle syncrone de 5 secondes
//uint32_t temps = 0;       //que dans le loop
//uint32_t GPStime = 0 ; //que dans le setup
//uint32_t FIXLED = 0;      //que dans le setup


uint8_t alerte=alerte_INIT;
uint8_t tour = 0;
uint8_t nombre = 0 ;
uint8_t batterie;

//bool SEND_ALL = 0;
bool alarmOccurredEAUP = false;
bool alarmOccurredMOVP = false;
bool alerte_BATP=false;
uint8_t NBCLK = 0;   //le reset est impossible, puisque l'on démare l'incrémenation de l'horloge à 1 a chaque boucle 
//bool etatledGPS = false;
//bool TRAMEGPS = false;
bool GPS_ENABLE = false;

float seuil_critique = 11.5; //tension critique de la batterie
bool delestage=0;   //si = 1 (oui) cela veut dire que la tension a tellement chuté que cela peut poser problème on arrete donc certain composants comme le gps
/*
11.2=0%
11.8=25%
12.2=50%
12.5=75%
12.9=100%
 */
 
//IN
const uint8_t PinEAU = 4;
const uint8_t PinMOV = 5;
const uint8_t PinCLK = 7;
const uint8_t PinBatt = A0;

//OUT
const uint8_t PinLEDProg = 0; 
const uint8_t PinLEDMMA = 1;
const uint8_t PinLEDLoRa = 2;
const uint8_t GPS_EN = 3;

const uint8_t PinLEDEAU = 9;
const uint8_t PinLEDMOV = 8;
const uint8_t PinLEDSENDMSG = 10;

const uint8_t PinLEDAlerteBat = 21; //si la tension de la batterie passe en dessous de seuil_critique alors on alume une led et on n'utilise plus le GPS
/*
           USB
A0 Batt
                  VCC
                  GND

                  14  TX
                  13  RX
21 PinLEDAlerteBat 12  SCL
0 PinLEDProg      11  SDA
1 PinLEDMMA       10  PinLEDSENDMSG
2 PinLEDLoRa      9   PinLEDMOV
3 GPS_EN          8   PinLEDEAU
4 EAU             7   CLK
5 MOV             6  ne pas utiliser (led sur le mkr)

led d'alimentation sur VCC
led CLK branché en sortie du NE555
led fix branché au module GPS
 */
 
