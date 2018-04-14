#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <assert.h>
#include "RfRemote.h"
#include "Ir.h"
#include "MqttCom.h"
#include "Ota.h"

void setupWifi();

void setup()
{
    Serial.begin(115200);
    Serial.println("\nBooting");

    setupWifi();
    setupOta();

    MqttCom.incomingTopic = "ithotest";
    MqttCom.setup();

    pinMode(2, OUTPUT);
    RfRemote.setup();

    setupIr();
}

void loop()
{
    ArduinoOTA.handle();
    RfRemote.loop();
    MqttCom.loop();
    loopIr();
}
