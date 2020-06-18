### Notes
note de cette version : 16/06/2020  V7.5
Fonctionne avec la canne n°1

### Reste à faire : 
- sleep
- gps
- mode dégradé = delestage
comment savoir si la canne est toujours connéctée au réseau ? 
le uplink n'est pas configuré dans ttn
reception du message retour 
si pas reçu alors on réenvoi le msg (1 ou 2 fois)

# Canne-avertisseuse
"La canne_avertisseuse" is an alert system for gravity irrigation. They use LoRa to communicate and detect the presence of water  
## Hardware


### Compenent

* Arduino MKR WAN 1300 - 28€(25/07/2019) https://bit.ly/2GuevrS
<img src=https://user-images.githubusercontent.com/32598441/61883824-679ede80-aefb-11e9-8246-e1c03fb2b4e8.jpg width="100" />

* Antenne for 868 MHZ - ~5€ https://bit.ly/2SDVVCL
<img src=https://user-images.githubusercontent.com/32598441/61883832-6a99cf00-aefb-11e9-862d-c804beff6443.jpg width="100" />

* GPS Adafruit 745 - 41€  https://bit.ly/2LHuB5R
<img src=https://user-images.githubusercontent.com/32598441/61883842-6e2d5600-aefb-11e9-97ed-8e78c9d13dd1.jpg width="100" />

*accelerometer Adafruit MMA8451 - 9€ https://bit.ly/2Mcii0F
<img src=https://user-images.githubusercontent.com/32598441/61883851-71284680-aefb-11e9-8e40-f462b3d68d67.jpg width="100" />

* 2× Female connector 14 in line -3,41€ https://fr.rs-online.com/web/p/fiches-femelles-pour-ci/7679347/ 
<img src=https://user-images.githubusercontent.com/32598441/61883903-8a30f780-aefb-11e9-9f2b-a45f07767ee2.jpg width="100" />

* Females connector 8 and 9 

* 2× terminal block connector https://fr.rs-online.com/web/p/products/1814377/
<img src=https://user-images.githubusercontent.com/32598441/61883918-8ef5ab80-aefb-11e9-98b9-c4e7c0803b27.jpg width="100" />

* regulateur DC/DC 4,6-32V ->3,3V https://fr.rs-online.com/web/p/regulateurs-a-decoupage/7553429/
<img src=https://user-images.githubusercontent.com/32598441/61883931-9321c900-aefb-11e9-8f70-7ebc4981d139.jpg width="100" />

## setting
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
* Water sensor: it is made up of 2 electrodes if the current manages to pass between the 2 electrodes, the input A6 goes from 0v to ~3.3V. The water is detected 
* Accelerometer: If one of the acceleration values changes sufficiently on one of the axes, it can be deduced that the rod is no longer in its initial position 
* GPS: The GPS gives us the geographical position of the device. A measurement is only taken at the start or when a sensor detects something 
* Voltmeter for the battery: the battery level is regularly monitored to prevent any system failure    






