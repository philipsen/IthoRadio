#include "IthoCC1101.h"

class IthoSenderClass
{
  public:
    void turnOff() { sendCommand("eco"); }
    void turnOn() { sendCommand("cook1"); }

    void sendCommand(const String &c);

    void remoteId(uint8_t*);
    
  private:
    static void _convertToPacket(const ByteArray &, CC1101Packet &);

    uint8_t _counter = 87;

    uint8_t _remoteId[3] = {0, 0, 0};
    uint8_t _remoteByte0 = 0x16;
};

extern IthoSenderClass IthoSender;