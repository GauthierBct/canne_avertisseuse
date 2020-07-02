### Notes
note de cette version : 30/06/2020	V8
Ce programme ne fonctionne qu'en présence d'une horloge externe. 

### Reste à faire : 
- comment savoir si la canne est toujours connectée au réseau ? 

# Canne-avertisseuse
"La canne_avertisseuse" is an alert system for gravity irrigation. They use LoRa to communicate and detect the presence of water  

## Hardware
<img src=https://raw.githubusercontent.com/hugonad/canne_avertisseuse/nouvelle-version-sommeil/exporteagle.png width="2000" />

### Compenent

* Arduino MKR WAN 1300 - 35€ HT https://store.arduino.cc/arduino-mkr-wan-1300-lora-connectivity-1414
 or Arduino MKR WAN 1310 - 33€ HT https://store.arduino.cc/mkr-wan-1310
<img src=https://user-images.githubusercontent.com/32598441/61883824-679ede80-aefb-11e9-8246-e1c03fb2b4e8.jpg width="100" />

* Antenna for 868 MHZ - ~3,9€ HT https://bit.ly/2SDVVCL
<img src=https://user-images.githubusercontent.com/32598441/61883832-6a99cf00-aefb-11e9-862d-c804beff6443.jpg width="100" />

* GPS Adafruit 745 - 35,53 € HT  https://bit.ly/2LHuB5R
<img src=https://user-images.githubusercontent.com/32598441/61883842-6e2d5600-aefb-11e9-97ed-8e78c9d13dd1.jpg width="100" />

* Antenna for GPS SMA SMA960 - 13,58€ HT https://www.gotronic.fr/art-antenne-gps-sma-sma960-20941.htm + adapateur 4,08 HT https://www.gotronic.fr/art-adaptateur-ufl-sma-20893.htm
essayer avec : Antenne GPS céramique active 3 HT (moins cher)

* accelerometer Adafruit MMA8451 - 6,95€ HT https://bit.ly/2Mcii0F
<img src=https://user-images.githubusercontent.com/32598441/61883851-71284680-aefb-11e9-8e40-f462b3d68d67.jpg width="100" />

* 2× Female connector 36 in line - 2,08 € HT https://www.gotronic.fr/art-connecteur-fh136z-4462.htm
<img src=https://user-images.githubusercontent.com/32598441/61883903-8a30f780-aefb-11e9-9f2b-a45f07767ee2.jpg width="100" />

* Porte-fusible PF3 - 0,21 € HT

* Plaque d'essais BCS160, 100 x 160 mm pastilles - 3,08 € HT

* 3× terminal block connector - 0,589 € HT https://fr.rs-online.com/web/p/products/1814377/
<img src=https://user-images.githubusercontent.com/32598441/61883918-8ef5ab80-aefb-11e9-98b9-c4e7c0803b27.jpg width="100" />

* regulateur DC/DC 4,6-32V ->3,3V - 4,68 HT https://fr.rs-online.com/web/p/regulateurs-a-decoupage/7553429/
a remplacer par (meilleur rendement et courant de veille) 1,92 HT https://fr.rs-online.com/web/p/regulateurs-a-decoupage/1933974/
<img src=https://user-images.githubusercontent.com/32598441/61883931-9321c900-aefb-11e9-8f70-7ebc4981d139.jpg width="100" />

* Case ~15€ 

* 2 waterproof switch - 2,33 € HT
* 10 leds - 0,33€ HT
* Inverseur unipolaire ESP101 - 0,79 € HT
* NE555 - 0,29€ HT

* Resistors and capacitors ~2€ :
270k,
120k,
100k,
56k,
10k,
2,7k,
100 µF,
330 nF,
100 nF

* Battery PS1270GB 12V 7,0Ah - au plomb - 19,92 € HT - https://www.gotronic.fr/art-batterie-ps1270gb-5655.htm

Total = 157,75€ HT

### Libraries

Librairies à installer :
- Adafruit GPS
- Adafruit MMA8451
- Adafruit Sensor (s'installe normalement avec Adafruit MMA8451)
- MKRWAN 
- ArduinoLowPower
- RTCZero

Pour installer les librairies ouvrir le gestionnaire grâce à CTRL+MAJ+I ou Croquis -> Inclure une Blibliothèque -> Gérer les Blibliothèques.
Rechercher dans la barre de recherche le nom de la blibliothèque à installer.

## Setting
to start compiling the program and uploading it. By reading the serial port you can get the ```DEV_ID``` to configure the device on The Things network.
<img src=https://user-images.githubusercontent.com/32598441/61884466-9073a380-aefc-11e9-9db9-392b89730151.PNG width="1000" />

### Commissioning.h
you must enter the keys APP_EUI and APP_KEY to retrieve from the The Things Board application.
to allow communication.

<img width="600" alt="APB" src="https://user-images.githubusercontent.com/32598441/61884974-5f47a300-aefd-11e9-8f04-65e8a9a95f3b.PNG">

```
#define APP_EUI "123456789ABCDEF"
#define APP_KEY "123456789ABCDEF"
```

The other parameters can be adjusted to adapt to the condition of use.   
The ABP parameters will only be completed in the case of an ABP connection. 
### canne_avertisseuse.h

...

## Principle of operation

### ALERT
* water alert
* motion/theft alert 
* battery alert
In the case of one of these alerts, a message containing the alert type, battery value and GPS coordinates is sent 
otherwise only a heartbeat is sent regularly 

### Sensor
* Water sensor: it is made up of 2 electrodes if the current manages to pass between the 2 electrodes, the input 4 goes from 3.3v to 0V  The water is detected 
* Accelerometer: If an acceleration is detected the mouvement is detected (the value of the acceleration is adjustable in the MMA8451_IRQ.h library). 

* GPS: The GPS gives us the geographical position of the device. A measurement is only taken at the start or when a sensor detects something 
* Voltmeter for the battery: the battery level is regularly monitored to prevent any system failure
