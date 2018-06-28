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
  String clientName;
  void logger(const String& m);
  
public:
  PubSubClient *_client;
  void _reconnect();
};

extern MqttComClass MqttCom;
