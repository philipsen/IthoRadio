#include <Arduino.h>

class MqttCom;
class ByteArray;
class CC1101Packet;

class RfRemoteClass
{
  public:
    RfRemoteClass() {}

    void setup();
    void loop();

    void attachIter();
    void detachIter();
    void resetBuffer();

    bool printAllPacket = false;
    bool printNonRemote = false;
    bool printDebug = false;

    void turnOn();
    void turnOff();

    //void convertToPacket(const ByteArray &, CC1101Packet &);
    static String toString(uint8_t *data, unsigned int length, bool ashex = true);

  private:
    bool _gotIthoStart;
    uint8_t _lastIndex;

    size_t oldSize = 0;
    size_t checkIdx = 0;
};

extern RfRemoteClass RfRemote;
