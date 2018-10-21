#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include "WifiSetup.h"

#include <IthoReceive.h>
#include "MqttCom.h"

#include "Ir.h"
#include "Ota.h"


#include <PubSubClient.h>

void setupWifi();
void setupWeb();
void loopWeb();

String remoteName = "ithoin";
const uint8_t keukenId[] = {0x52, 0x50, 0xb9};
const uint8_t badkamerId[] = {0x74, 0xf3, 0xaf};

void logger(const String& m)
{
    MqttCom.logger(m);
}


void setup()
{
    Serial.begin(115200);
    Serial.println("\nBooting");

    setupWifi(false);

    if (MDNS.begin("ithoremote"))
        Serial.println("mDNS responder started");
    else
        Serial.println("Error setting up MDNS responder!");

    setupOta();

    setupWeb();

    MqttCom.clientName = house_token;
    MqttCom.incomingTopic = String(house_token) + "/+/+";
    MqttCom.incomingTopic2 = String("itho/") + String(house_token) + "/#";
    MqttCom.setup();

    Serial.println("IthoReceive.setInterruptPin");
    IthoReceive.setInterruptPin(2);
    IthoReceive.printAllPacket = false;
    IthoReceive.printNonRemote = false;

    IthoReceive.logger(logger);
    IthoSender.logger(logger);

    IthoReceive.setup();
    setupIr();

    logger("setup done");
}

void loop()
{
    ArduinoOTA.handle();
    IthoReceive.loop();
    loopWeb();
    MqttCom.loop();
    loopIr();
}
