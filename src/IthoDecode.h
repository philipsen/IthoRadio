#include <Arduino.h>
#include "BitArray.h"

class DemandIthoCommand;

class IthoDecode
{
  public:
    static ByteArray encode(DemandIthoCommand& cmd);

    static String decode(uint8_t *data, uint8_t length);
    static String decode2(uint8_t *data, uint8_t length);

    static uint8_t crc(const String& rawByteString);
    static String toPrintString(const String& s);

private:
    static const uint8_t _preamble[];
    static const uint8_t _postamble[];
};