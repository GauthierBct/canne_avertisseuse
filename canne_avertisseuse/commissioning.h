#define OVER_THE_AIR_ACTIVATE 1
/*OTAA->1
 *ABP->0
 */
#define DutyCycle     1 // sending 1% of time ( 1 by default)
#define PublicNetwork 1 // 1 by default
#define DataRate      0// sending speed
/*
0- long range and low sending speed
5- small range and high sending speed
*/
#define ADR           1  //adaptative data rate(1 by default)
/*set max capacity to long range 
 * 0 0 0 0
 */
/*OTAA connection adresse*/
/*--------------------------------------------*/
#define APP_EUI "70B3D57ED001D6F7"
#define APP_KEY "52532EE81072BE1DE42AF39EC734C6D5"
/*------------------------------------------*/
/*ABP connection adresss
--------------------------------------------*/
#define DEVADDR ""
#define NWKSKEY ""
#define APPSKEY ""
/*-----------------------------------------*/
