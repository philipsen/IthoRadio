#ifndef __IthoCommand__
#define __IthoCommand__

#include <Arduino.h>
#include "ByteArray.h"

class IthoCommand
{
    public:
        IthoCommand(uint8_t leadByte, ByteArray id, uint8_t counter, ByteArray command);

        IthoCommand(const String& s) {
            _lead.substring(s, 0, 1);
            _id.substring(s, 1, 4);
            _counter.substring(s, 4, 5);
            _command.substring(s, 5, s.length()-1);
            _crc.substring(s, s.length()-1, s.length());
        }

        uint8_t sum() { return _lead.sum() + _id.sum() + _counter.sum() + _command.sum() + _crc.sum(); }
        String toString();

        const ByteArray& lead() const { return _lead; }
        const ByteArray& id() const { return _id; }
        const ByteArray& counter() const { return _counter; }
        const ByteArray& command() const { return _command; }
        const ByteArray& crc() const { return _crc; }

    private:
        ByteArray _lead;
        ByteArray _id;
        ByteArray _counter;
        ByteArray _command;
        ByteArray _crc;

        void _setCrc();

};

#endif // !1
