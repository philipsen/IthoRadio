#include <Arduino.h>
#include "RfRemote.h"
#include "MqttCom.h"
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

WiFiClient espClient;

MqttCom *_globalMqtt = 0;

//print any message received for subscribed topic
void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = 0;
    printf("mqtt msg arrived: %s(%d) -> %s\n", topic, length, payload);
    // int delaytime = 500;
    // uint8_t maxTries = 3;

    // todo: check incoming topic
    if (_globalMqtt != 0 && _globalMqtt->incomingTopic == topic)
    {
        String c = String((char *)payload);
        RfRemote::_globalRf->sendCommand(c);
    }
}

MqttCom::MqttCom(const String &t) : incomingTopic(t)
{
    _client = new PubSubClient(espClient);
    _globalMqtt = this;
}

void MqttCom::setup()
{
    //connect to MQTT server
    _client->setServer("pi3.lan", 1883);
    _client->setCallback(callback);
}

void MqttCom::loop()
{
    // put your main code here, to run repeatedly:
    if (!_client->connected())
    {
        _reconnect();
    }
    _client->loop();
}

void MqttCom::publish(const char *c, const char *m)
{
    _client->publish(c, m);
}

void MqttCom::_reconnect()
{
    // Loop until we're reconnected
    while (!_client->connected())
    {
        //Serial.print("Attempting MQTT connection...");
        // Attempt to connect, just a name to identify the client
        if (_client->connect("IthoControl2"))
        {
            //Serial.println("connected");
            // Once connected, publish an announcement...
            // client.publish("outpic","Hello World");
            // ... and resubscribe
            if (incomingTopic != "")
            {
                _client->subscribe(incomingTopic.c_str(), 0);
            }
            String m = String("connected ip = ") + String(WiFi.localIP());
            _client->publish("ithocontrol/log", m.c_str());
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(_client->state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
        //printf("connected = %d\n", _client->connected());
    }
}