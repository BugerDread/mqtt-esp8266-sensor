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
const String mqttbase = "idk/whatever/chodba/teplomer/";
const String mqtttemp = mqttbase + "teplota";
const String mqttpressure = mqttbase + "tlak";
const String mqttrssi = mqttbase + "signal";
const String mqttbatt = mqttbase + "baterie";

// config sensor hardware used
const byte bmp_sda = 4;   // SDA of LM75 = gpio4
const byte bmp_scl = 5;   // SCL of LM75 = gpio5
#include "sensor_bmp180.h"

//calibration constant for adc divider - known voltage in mv (as float!) / known adc value (caladc)
#define adconst 3280.0/784
#define vcrit 3200                      //critical battery voltage in mV, will power off without sending data if lower
#define vlow 3400                       //low battery voltage in mV, will send data but will sleep [lowbattsleep] if lower
