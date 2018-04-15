#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <assert.h>
#include <IthoReceive.h>
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

    IthoSender.remoteId({0x52, 0x50, 0xb9});

    IthoReceive.setInterruptPin(2);
    IthoReceive.printAllPacket = false;
    IthoReceive.printNonRemote = false;
    IthoReceive.setup();

    setupIr();
}

void loop()
{
    ArduinoOTA.handle();
    IthoReceive.loop();
    MqttCom.loop();
    loopIr();
}
