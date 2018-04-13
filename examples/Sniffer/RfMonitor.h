#include <Arduino.h>

class MqttCom;
class ByteArray;
class CC1101Packet;

class RfMonitor
{
  public:
    void setup();
    void loop();
    void convertToPacket(const ByteArray&, CC1101Packet&);

    void attachIter();
    void detachIter();
    void resetBuffer();
    
  private:
    bool _gotIthoStart;
    uint8_t _lastIndex;
};

