//reset button on EN input clears RTC also
//holding FLASH button after reset initiates OTA update - led will be flashing

//OTA je zatim v teplomeru na chodbe

#include <ESP8266WiFi.h>
#include <MQTT.h>
//OTA
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

static const char OTA_PWD[] = "OTApassword";

WiFiClientSecure net;
MQTTClient client;

#define lowbattsleep 3600e6   //sleep hour if battery low
#define delaysleep 170e6     //delay between sending next data
#define maxfail 3             //number of WiFi/mqtt connection failures to sleep for "cerrsleep"
#define mqtt_connect_retry 2  //number of tries to connect to mqtt server in 1 round (every connect takes about 12sec regardles of settimeout())
#define cerrsleep 600e6      //delay between sending next data when can connect maxfail times
#define vcrit 3100                      //critical battery voltage in mV, will power off without sending data if lower
#define vlow 3200                       //low battery voltage in mV, will send data but will sleep [lowbattsleep] if lower

//here we include sensor config
//include "cfg_zachod.h"
//#include "cfg_puda.h
#include "cfg_spiz.h"
//#include "cfg_chodba.h"
//#include "cfg_koupelna.h"
//#include "cfg_koupelna_htu.h"
//#include "cfg_zahradka.h"
//#include "cfg_puda.h"
//#include "cfg_kuchyn.h"
//#include "dread.h"

#ifndef adconst
ADC_MODE(ADC_VCC);
#endif

uint16_t vcc;
unsigned long startmillis = 0;

// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
struct {
  uint32_t crc32;   // 4 bytes
  uint8_t channel;  // 1 byte,   5 in total
  uint8_t ap_mac[6]; // 6 bytes, 11 in total
  uint8_t fail_count;  // 1 byte,  12 in total
} rtcData;

void sleepme() {
  sensor_off();
  if (client.connected()) {
    client.disconnect();
  }
  WiFi.disconnect();
  unsigned long stime;
  if (vcc < vlow) {
    stime = lowbattsleep; // sleep longer, battery low
  } else {
    stime = delaysleep;
  }
  Serial.printf("Round time: %dms\r\nGoing to sleep for %ds", millis() - startmillis, stime / 1000000);
  ESP.deepSleep(stime);
}

uint32_t calculateCRC32( const uint8_t *data, size_t length ) {
  uint32_t crc = 0xffffffff;
  while ( length-- ) {
    uint8_t c = *data++;
    for ( uint32_t i = 0x80; i > 0; i >>= 1 ) {
      bool bit = crc & 0x80000000;
      if ( c & i ) {
        bit = !bit;
      }
      crc <<= 1;
      if ( bit ) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

void writertc() {
  rtcData.crc32 = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
  ESP.rtcUserMemoryWrite( 0, (uint32_t*)&rtcData, sizeof( rtcData ) );
}

void sleepme_conn_fail(bool rtcValid){
  Serial.println(F("\r\nGiving up and prepare to sleep"));
  if (rtcValid) {
    //so the fail counter is valid
    if (rtcData.fail_count < 255) {   //down allow to overflow
      rtcData.fail_count++;           //count fails
    }
    if (rtcData.fail_count < maxfail) {
      //still did not reached maxfail... add one, write into RTC and sleep as usuall
      Serial.printf_P(PSTR("connection fail #%u, still did not reached maxfail\r\n"), rtcData.fail_count);
      writertc();
      sleepme();
    } else {
      //reached maxfail, sleep longer
      Serial.printf_P(PSTR("connection fail #%u, maxfail reached\r\n"), rtcData.fail_count);
      sensor_off();
      ESP.deepSleep(cerrsleep);
    }
  } else {
    //rtc not valid, fail_count set to 1, channel to 0, write to rtc and sleep as usuall
    Serial.println(F("rtc not valid, fail_count set to 1, channel to 0, write to rtc and sleep as usuall"));
    rtcData.fail_count = 1;
    rtcData.channel = 0;
    writertc();
    sleepme();
  }
}

bool check_ota_button() {     //return true if OTA button pressed
    if (digitalRead(0) == HIGH) {
        //no the button is not pressed
        return false;
    }

    //yes, button pressed, init OTA
    Serial.println(F("OTA button pressed, OTA init"));
    // ArduinoOTA.setPort(8266);                                                  // Port defaults to 8266
    // ArduinoOTA.setHostname("myesp8266");                                       // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setPassword(OTA_PWD);                                    // No authentication by default
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3                              // Password can be set with it's md5 value as well
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = F("sketch");
        } else { // U_FS
            type = F("filesystem");                                                   // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        }
        Serial.printf_P(PSTR("Start updating %s\r\n"), type.c_str());
    });

    ArduinoOTA.onEnd([]() {
        Serial.println(F("\r\nEnd"));
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf_P(PSTR("Error[%u]: "), error);
        if (error == OTA_AUTH_ERROR) {
          Serial.println(F("Auth Failed"));
        } else if (error == OTA_BEGIN_ERROR) {
          Serial.println(F("Begin Failed"));
        } else if (error == OTA_CONNECT_ERROR) {
          Serial.println(F("Connect Failed"));
        } else if (error == OTA_RECEIVE_ERROR) {
          Serial.println(F("Receive Failed"));
        } else if (error == OTA_END_ERROR) {
          Serial.println(F("End Failed"));
        }
    });

    ArduinoOTA.begin();
    Serial.print(F("OTA ready\r\nIP address: "));
    Serial.println(WiFi.localIP());
    //analogWrite(LED_BUILTIN, 127);
    return true;
}

void setup() {
  Serial.begin(115200);

#ifndef adconst
  vcc = ESP.getVcc();
#else
  vcc = round(float(analogRead(A0)) * adconst);
#endif

  Serial.printf("\r\n\r\n# # # B O O T # # #\r\nBattery check: %dmV\r\n", vcc);
  if (vcc < vcrit) {
    Serial.println(F("Voltage too low, shutting down!"));
    ESP.deepSleep(lowbattsleep);
  }

  startmillis = millis();

  pinMode(0, INPUT_PULLUP);   //button to init OTA
  pinMode(LED_BUILTIN, OUTPUT);
  analogWriteRange(255);
  analogWriteFreq(100);
  analogWrite(LED_BUILTIN, 254);

  sensor_init();

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
#ifndef useDHCP
  WiFi.config(staticIP, gateway, subnet);
#endif
  // Try to read WiFi settings from RTC memory
  Serial.print("Try to read WiFi settings from RTC memory: ");
  bool rtcValid = false;
  if ( ESP.rtcUserMemoryRead( 0, (uint32_t*)&rtcData, sizeof( rtcData ) ) ) {
    // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
    uint32_t crc = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
    if ( crc == rtcData.crc32 ) {
      // The RTC data good, make a quick connection
      rtcValid = true;
      Serial.printf("SUCCESS !\r\nRTC data: BSSID: %02x:%02x:%02x:%02x:%02x:%02x channel: %u fail_count: %u\r\n", rtcData.ap_mac[0], rtcData.ap_mac[1], rtcData.ap_mac[2], rtcData.ap_mac[3], rtcData.ap_mac[4], rtcData.ap_mac[5], rtcData.channel, rtcData.fail_count);
      if (rtcData.channel != 0) {
        Serial.printf("Connecting to BSSID: %02x:%02x:%02x:%02x:%02x:%02x channel: %u\r\n", rtcData.ap_mac[0], rtcData.ap_mac[1], rtcData.ap_mac[2], rtcData.ap_mac[3], rtcData.ap_mac[4], rtcData.ap_mac[5], rtcData.channel);
        WiFi.begin(wifi_ssid, wifi_pass, rtcData.channel, rtcData.ap_mac, true);
      } else {
        Serial.println(F("OK, but no channel / BSSID data found, connecting without BSSID and channel"));
        WiFi.begin(wifi_ssid, wifi_pass);
      }
    } else {
      // The RTC data was not valid, so make a regular connection
      Serial.println(F("FAIL !\r\nConnecting without BSSID and channel"));
      WiFi.begin(wifi_ssid, wifi_pass);
    }
  }

  int tmr = 0;
  while ( WiFi.status() != WL_CONNECTED ) {
    tmr++;
    Serial.print(".");

    if ( tmr == 100 ) {
      // Quick connect is not working, reset WiFi and try regular connection
      Serial.println(F("\r\nQuick connect is not working, reset WiFi and try regular connection"));
      WiFi.disconnect();
      delay( 10 );
      WiFi.forceSleepBegin();
      delay( 10 );
      WiFi.forceSleepWake();
      delay( 10 );
      WiFi.begin(wifi_ssid, wifi_pass);
    }
    if ( tmr == 300 ) {
      // Giving up after 15 seconds and going back to sleep
      sleepme_conn_fail(rtcValid);
    }
    delay( 50 );
  }

  //here we are connected to WiFi

  // Write current connection info to RTC
  if (!rtcValid) {
    //reset the fail counter if RTC is invalid, but keep if it is valid
    rtcData.fail_count = 0;
  }
  rtcData.channel = WiFi.channel();
  memcpy( rtcData.ap_mac, WiFi.BSSID(), 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
  writertc();   //since now the rtc is valid
  Serial.printf("\r\nConnected to BSSID: %02x:%02x:%02x:%02x:%02x:%02x channel: %u\r\n", rtcData.ap_mac[0], rtcData.ap_mac[1], rtcData.ap_mac[2], rtcData.ap_mac[3], rtcData.ap_mac[4], rtcData.ap_mac[5], rtcData.channel);

  if (check_ota_button()) {
      //button pressed
      return;
  }

  //FLASH button NOT pressed, send data to boker
  Serial.println("Connecting MQTT broker");
  analogWrite(LED_BUILTIN, 253);

  net.setInsecure();  //needed for esp8266 core 2.5.x +
  client.begin(mqtthost, mqttport, net);
  tmr = 0;
  while (!client.connect(mqttname, mqttuser, mqttpass)) {   //one try of connect takes about 10s if server NA, regardles of timeout
    tmr++;
    Serial.printf_P(PSTR("MQTT connection failed %d times\r\n"), tmr);
    if (tmr > mqtt_connect_retry) {                                          
      Serial.println(F("MQTT unavailable, going sleep"));
      sleepme_conn_fail(true);    //rtc is valid sice we wrote it after succesfull wifi connection
    }
  }

  Serial.println("MQTT CONNECTED!");
  rtcData.fail_count = 0;   //reset fail counter
  writertc();
  analogWrite(LED_BUILTIN, 251);
  sensor_send();
  client.publish(mqttrssi, String(WiFi.RSSI()), true, 1); // send also WiFi RSSI
  if (mqttbatt != "") {
    client.publish(mqttbatt, String(vcc), true, 1); //and voltage if mqttbat not empty
  }
  sleepme();
}

unsigned long m, nextmillis = 0;
uint8_t ledbr = 0;
void loop() {
  //this runs only in OTA mode
  ArduinoOTA.handle();
  m = millis();
  if (m > nextmillis) {
    //do sth with the led to indicate OTA mode
    nextmillis = m + 10;
    analogWrite(LED_BUILTIN, ledbr++);
  }
}

//if reset button connected to EN instead of RST it clears RTC when reset is pressed
//https://www.bakke.online/index.php/2017/06/24/esp8266-wifi-power-reduction-avoiding-network-scan/
