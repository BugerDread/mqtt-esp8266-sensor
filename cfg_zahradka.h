// config WiFi
const char wifi_ssid[] = "ssid";
const char wifi_pass[] = "wifipassword";

// config network
IPAddress staticIP(192, 168, x, x);
IPAddress gateway(192, 168, x, x);
IPAddress subnet(255, 255, 255, 0);

// config mqtt connection
const char mqtthost[] = "192.168.x.x";
const char mqttname[] = "name";
const char mqttuser[] = "username";
const char mqttpass[] = "pwd";
const uint16_t mqttport = 8883;
const String mqttbase = "idk/whatever/zahradka/teplomer/";
const String mqtttemp = mqttbase + "teplota";
const String mqttpressure = mqttbase + "tlak";
const String mqttrssi = mqttbase + "signal";
const String mqttbatt = mqttbase + "baterie";;

// config sensor hardware used
const byte lmsda = 5;
const byte lmscl = 4;
const byte lmsubaddr = 0b111; // if A0, A1, A2->Vcc
#include "sensor_lm75.h"

// it is also possible to reconfigure "constants" (which can be constatnts because of that)
//#define delaysleep 170e6     //delay between sending next data

//calibration constant for adc divider - known voltage in mv (as float!) / known adc value (caladc)
#define adconst 4135.0/788
#define vcrit 3200                      //critical battery voltage in mV, will power off without sending data if lower
#define vlow 3400                       //low battery voltage in mV, will send data but will sleep [lowbattsleep] if lowe
