#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <IthoReceive.h>
#include "Ota.h"

void setupWifi();

void setup()
{
    Serial.begin(115200);
    Serial.println("\nBooting");

    setupWifi();
    setupOta();

    IthoReceive.setInterruptPin(2);
    IthoReceive.printAllPacket = true;
    IthoReceive.printNonRemote = true;
    IthoReceive.setup();
}

void loop()
{
    ArduinoOTA.handle();
    IthoReceive.loop();
}
