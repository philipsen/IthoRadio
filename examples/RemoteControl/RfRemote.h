#include <Arduino.h>

class MqttCom;
class ByteArray;
class CC1101Packet;

class RfRemote
{
  public:
    RfRemote() { _globalRf = this; }

    void setup();
    void loop();
    void convertToPacket(const ByteArray &, CC1101Packet &);

    void attachIter();
    void detachIter();
    void resetBuffer();

    bool printAllPacket = false;
    bool printNonRemote = false;
    bool printDebug = false;

    void turnOn();
    void turnOff();
    void sendCommand(const String &c);

    static RfRemote *_globalRf;

  private:
    bool _gotIthoStart;
    uint8_t _lastIndex;

    uint8_t _counter = 78;
};
