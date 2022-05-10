//define ampin before including this file
//example:
//const byte ampin = 14;

#include <DHTesp.h>

DHTesp dht;

void sensor_init() {
  dht.setup(ampin, DHTesp::AM2302);
}

void sensor_off() {
  //nothing to do here
}

void sensor_send() {
  TempAndHumidity sensor1Data = dht.getTempAndHumidity();
  String temperature = String(sensor1Data.temperature, 1);  //dht.getTemperature();
  String humidity = String(sensor1Data.humidity, 1);        //dht.getHumidity();  
  Serial.println(String(F("Temperature: ")) + temperature + F("C\nHumidity: ") + humidity + F("%"));

  client.publish(mqtttemp, temperature, true, 1);
  client.publish(mqtthumi, humidity, true, 1);
}
