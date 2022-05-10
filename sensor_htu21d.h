// define GPIO pins used to connect htu21d before including this file
// example:
// const byte htu_sda = 4;           // SDA = gpio4
// const byte htu_scl = 5;          // SCL = gpio5
// #include "sensor_htu21d.h"

#include <Wire.h>
#include "SparkFunHTU21D.h"

HTU21D htusensor;

void sensor_init() {
  Wire.begin(htu_sda, htu_scl);
  htusensor.begin();
}

void sensor_send() {
  float htu_temp = htusensor.readTemperature();  
  float htu_humi = htusensor.readHumidity();
  Serial.printf("Temperature: %.1fC\r\nHumidity: %.1f%\r\nPublishing data\r\n", htu_temp, htu_humi);
  client.publish(mqtttemp, String(htu_temp, 1), true, 1);
  client.publish(mqtthumi, String(htu_humi, 1), true, 1);
}

void sensor_off() {
  //nothing to do here
}
