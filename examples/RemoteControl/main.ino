#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>

#include <IthoReceive.h>
#include "MqttCom.h"

#include "Ir.h"
#include "Ota.h"

#include <PubSubClient.h>

void setupWifi();
void setupWeb();

String remoteName = "ithotest";

void logger(const String& m)
{
    MqttCom.publish("wmt6/log", m.c_str());
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

    MqttCom.incomingTopic = "wmt6/+/command";
    MqttCom.setup();
    MqttCom.loop();

    IthoReceive.setInterruptPin(2);
    IthoReceive.printAllPacket = false;
    IthoReceive.printNonRemote = false;
    IthoReceive.setup();
    IthoSender.logger(logger);

    setupIr();

    logger("setup done");
}

void loop()
{
    ArduinoOTA.handle();
    IthoReceive.loop();
    MqttCom.loop();
    loopIr();
}
