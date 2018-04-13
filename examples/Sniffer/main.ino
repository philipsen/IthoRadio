#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <assert.h>

#include "RfMonitor.h"

//#include "MqttCom.h"
//#include "Network.h"
#include "Ota.h"

RfMonitor rfmon;

//WiFiClient espClient;

//MqttCom mqtt("ithomon");

void setupWifi();

void callback(char*, byte*, unsigned int )
{
  Serial.println("mqtt callback");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nBooting");

  setupWifi();
  setupOta();
  pinMode(2, OUTPUT);
  rfmon.setup();
}

void loop()
{
  ArduinoOTA.handle();
  rfmon.loop();
}


