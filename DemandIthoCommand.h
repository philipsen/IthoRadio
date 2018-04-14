#ifndef __DemandIthoCommand__
#define __DemandIthoCommand__

#include <Arduino.h>
#include "ByteArray.h"

class DemandIthoCommand
{
    public:
        DemandIthoCommand(ByteArray id, uint8_t counter, ByteArray command);

        DemandIthoCommand(const String& s) {
            //printf("DemandIthoCommand ctor\n");
            _id.substring(s, 0, 4);
            _counter.substring(s, 4, 5);
            _command.substring(s, 5, s.length()-1);
            _crc.substring(s, s.length()-1, s.length());
        }

        uint8_t sum()
        {
            return _id.sum() + _counter.sum() + _command.sum() + _crc.sum();
        }

        String toString()
        {
            uint8_t crc = sum();
            
            String r = "i = ";
            r += _id.toString();
            r += " cn = ";
            r += _counter.toString();
            r += " cmd = ";
            r += _command.toString();
            r += " cs = ";
            r += _crc.toString();
            r += " cr = " + String(crc);
            return r;
        }

        const ByteArray& id() const { return _id; }
        const ByteArray& counter() const { return _counter; }
        const ByteArray& command() const { return _command; }
        const ByteArray& crc() const { return _crc; }

    private:
        ByteArray _id;
        ByteArray _counter;
        ByteArray _command;
        ByteArray _crc;

        void _setCrc();

};

#endif // !1
