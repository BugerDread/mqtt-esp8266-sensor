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
const String mqttbase = "idk/whatever/koupelna/teplomer/";
const String mqtttemp = mqttbase + "teplota";
const String mqttpressure = mqttbase + "tlak";
const String mqttrssi = mqttbase + "signal";
const String mqttbatt = mqttbase + "baterie";


// config sensor hardware used
const byte ampin = 14;
#include "sensor_am2302.h"

// it is also possible to reconfigure "constants" (which can be constatnts because of that)
//#define delaysleep 170e6     //delay between sending next data
