#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <assert.h>

#include "RfRemote.h"

RfRemote rfmon;

#include "Ir.h"

#include "MqttCom.h"
//#include "Network.h"
#include "Ota.h"

//WiFiClient espClient;

MqttCom mqtt("ithotest");

void setupWifi();

// void callback(char*, byte*, unsigned int )
// {
//   Serial.println("mqtt callback");
// }

void setup()
{
  Serial.begin(115200);
  Serial.println("\nBooting");

  mqtt.setup();

  setupWifi();
  setupOta();
  pinMode(2, OUTPUT);
  rfmon.setup();

  setupIr();
}

void loop()
{
  ArduinoOTA.handle();
  rfmon.loop();

  mqtt.loop();
  loopIr();
}


