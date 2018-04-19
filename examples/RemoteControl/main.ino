#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <assert.h>
#include <IthoReceive.h>
#include "Ir.h"
#include "MqttCom.h"
#include "Ota.h"

void setupWifi();

String remoteName = "ithotest";

void logger(const String& m)
{
    MqttCom.publish((String("itholog/") + remoteName).c_str(), m.c_str());
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\nBooting");

    setupWifi();
    setupOta();

    MqttCom.incomingTopic = remoteName.c_str();
    MqttCom.setup();

    IthoReceive.setInterruptPin(2);
    IthoReceive.printAllPacket = false;
    IthoReceive.printNonRemote = true;
    IthoReceive.setup();

    IthoSender.logger(logger);

    setupIr();
}

void loop()
{
    ArduinoOTA.handle();
    IthoReceive.loop();
    MqttCom.loop();
    loopIr();
}
