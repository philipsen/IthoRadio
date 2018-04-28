#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>

#include <IthoReceive.h>
#include "Ir.h"
#include "MqttCom.h"
#include "Ota.h"

void setupWifi();
void setupWeb();

String remoteName = "ithotest";

void logger(const String& m)
{
    MqttCom.publish((String("itholog/") + remoteName).c_str(), m.c_str());
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\nBooting");

    WiFiManager wifiManager;    //wifiManager.resetSettings();
    wifiManager.autoConnect("AutoConnectAP", "123456");
    if (MDNS.begin("ithoremote"))
        Serial.println("mDNS responder started");
    else
        Serial.println("Error setting up MDNS responder!");

    setupOta();

    setupWeb();

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
