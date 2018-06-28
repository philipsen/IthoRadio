#include <Arduino.h>
#include "IthoSender.h"
#include "MqttCom.h"
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

WiFiClient espClient;

//print any message received for subscribed topic
void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = 0;
    printf("mqtt msg arrived: %s(%d) -> %s\n", topic, length, payload);

    // todo: check incoming topic
    String c = String((char *)payload);
    IthoSender.sendCommand(c);
}

MqttComClass::MqttComClass(const String &t) : incomingTopic(t)
{
    _client = new PubSubClient(espClient);
}

void MqttComClass::setup()
{
    //connect to MQTT server
    _client->setServer("gc.cwvzuidpoort.org", 1883);
    _client->setCallback(callback);

    // force connecting
    loop();
}

void MqttComClass::loop()
{
    // put your main code here, to run repeatedly:
    if (!_client->connected())
    {
        _reconnect();
    }
    if (_client->connected())
        _client->loop();
}

void MqttComClass::publish(const char *c, const char *m)
{
    _client->publish(c, m);
}

void MqttComClass::_reconnect()
{
    // Loop until we're reconnected
    while (!_client->connected())
    {
        //Serial.print("Attempting MQTT connection...");
        // Attempt to connect, just a name to identify the client
        if (_client->connect(clientName.c_str()))
        {
            //Serial.println("connected");
            // Once connected, publish an announcement...
            // client.publish("outpic","Hello World");
            // ... and resubscribe
            if (incomingTopic != "")
            {
                _client->subscribe(incomingTopic.c_str(), 0);
            }
            char buf[20];
            IPAddress ip = WiFi.localIP();
            sprintf(buf, "ip: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

            String m = String("connected ip = ") + String(WiFi.localIP());
            logger(String("topic=") + incomingTopic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(_client->state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
        printf("re connected = %d\n", _client->connected());
    }
}

void MqttComClass::logger(const String& m)
{
    publish((clientName + String("/log")).c_str(), m.c_str());
}

MqttComClass MqttCom("ithoin");
