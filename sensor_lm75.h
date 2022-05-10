// define GPIO pins used to connect LM75 and lmsubaddr before including this file
// example:
// const byte lmsda = 4;           // SDA of LM75 = gpio4
// const byte lmscl = 5;          // SCL of LM75 = gpio5
// const byte lmsubaddr = 0b111;  // if A0, A1, A2->Vcc
// #include "sensor_lm75.h"

#include <Wire.h>
#include <LM75.h>

LM75 sensor(LM75_ADDRESS | lmsubaddr);  // if A0, A1, A2->Vcc

void sensor_init() {
  Wire.begin(lmsda,lmscl);
  sensor.shutdown(false);
}

void sensor_send() {
  float lmtemp = sensor.temp();   //one decimal place is enough, afaik there are LM75s with resolution 0.5C and others with 0.125C ... 
  //Serial.println(String(F("Temperature ")) + lmtemp + String(F("C")) + String(F("\nPublishing data")));
  Serial.printf("Temperature %.1fC\r\nPublishing data\r\n", lmtemp);
  client.publish(mqtttemp, String(lmtemp, 1), true, 1);
}

void sensor_off() {
  sensor.shutdown(true);
}
