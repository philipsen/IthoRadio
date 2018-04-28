#include <Arduino.h>
#include <ESP8266WiFi.h>

class PubSubClient;

class MqttComClass
{
public:
  MqttComClass(const String& t);
  void setup();
  void loop();
  void publish(const char *c, const char *m);
  String incomingTopic;

private:
  PubSubClient *_client;
  void _reconnect();

  String _toStringIp(IPAddress ip) {
    String res = "";
    for (int i = 0; i < 3; i++) {
      res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
  }
};

extern MqttComClass MqttCom;
