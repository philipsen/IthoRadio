#include "IthoCC1101.h"

class IthoSenderClass
{
  public:
    void turnOff() { sendCommand("eco"); }
    void turnOn() { sendCommand("cook1"); }

    void sendCommand(const String &c);

  private:
    static void _convertToPacket(const ByteArray &, CC1101Packet &);

    uint8_t _counter = 87;
};

extern IthoSenderClass IthoSender;