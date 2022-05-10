// define GPIO pins used to connect BMPxxx before including this file
// example:
// const byte bmp_sda = 4;           // SDA = gpio4
// const byte bmp_scl = 5;          // SCL = gpio5
// #include "sensor_bmp180.h"

#include <Wire.h>
#include <Sodaq_BMP085.h>

Sodaq_BMP085 bmp;

void sensor_init() {
  Wire.begin(bmp_sda, bmp_scl);
  bmp.begin(BMP085_ULTRAHIGHRES);
}

void sensor_send() {
  //String bmp_temp = String(bmp.readTemperature(), 1);   //one decimal place is enough
  //String bmp_pressure = String(bmp.readPressure());
  float bmp_temp = bmp.readTemperature();
  int32_t bmp_pressure = bmp.readPressure();
  
  Serial.printf("Temperature: %.1fC\r\nPressure: %dPa\r\nPublishing data\r\n", bmp_temp, bmp_pressure);
  client.publish(mqtttemp, String(bmp_temp, 1), true, 1);
  client.publish(mqttpressure, String(bmp_pressure), true, 1);
}

void sensor_off() {
  //nothing to do here
}
