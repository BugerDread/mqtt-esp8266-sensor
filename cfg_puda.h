// config WiFi
const char wifi_ssid[] = "Garden WiFi";
const char wifi_pass[] = "badik2323";

// config network
IPAddress staticIP(192,168,88,29);
IPAddress gateway(192,168,88,1);
IPAddress subnet(255,255,255,0);

const char mqtthost[] = "192.168.88.226";
const uint16_t mqttport = 8883;
const char mqttname[] = "esptpuda";
const char mqttuser[] = "esptpuda";
const char mqttpass[] = "pudiCKA2048Q";
String mqttbase = "pop/puda/teplomer/";
String mqtttemp = mqttbase + "teplota";
String mqttrssi = mqttbase + "signal";
String mqttbatt = "";   //empty to disable voltage reporting - not needed, powered from PC

// config sensor hardware used
const byte lmsda = 4;   // SDA of LM75 = gpio4
const byte lmscl = 5;   // SCL of LM75 = gpio5
const byte lmsubaddr = 0b000; // if A0, A1, A2->Gnd
#include "sensor_lm75.h"

// it is also possible to reconfigure "constants"
// #define delaysleep 170e6     //delay between sending next data
//sensor powered from PC, makes no sense to check voltages
#define vcrit 0                      //critical battery voltage in mV, will power off without sending data if lower
#define vlow 0                       //low battery voltage in mV, will send data but will sleep [lowbattsleep] if lower
