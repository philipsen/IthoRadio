#include <Arduino.h>

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
};

extern MqttComClass MqttCom;
