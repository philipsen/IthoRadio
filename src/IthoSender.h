#include "IthoCC1101.h"

class IthoSenderClass
{
  public:
    void turnOff() { sendCommand("eco"); }
    void turnOn() { sendCommand("cook1"); }

    void sendCommand(const String &c);

    void remoteId(uint8_t*);

    void logger(void (*) (const String&));

  private:
    static void _convertToPacket(const ByteArray &, CC1101Packet &);
    void (* _logger) (const String&) = NULL;   
    void _log(const String &s);

    uint8_t _counter = 87;
    uint8_t _remoteId[3] = {0x52, 0x50, 0xb9};
    uint8_t _remoteByte0 = 0x16;
};

extern IthoSenderClass IthoSender;