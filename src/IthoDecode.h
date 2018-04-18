#include <Arduino.h>
#include "BitArray.h"

class IthoCommand;

class IthoDecode
{
  public:
    static ByteArray encode(IthoCommand& cmd);
    static String decode(uint8_t *data, uint8_t length);

    static uint8_t crc(const String& rawByteString);
    static String toPrintString(const String& s);

private:
    static const uint8_t _preamble[];
    static const uint8_t _postamble[];
};