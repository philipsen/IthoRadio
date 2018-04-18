#include "IthoCommand.h"

IthoCommand::IthoCommand(uint8_t lead, ByteArray id, uint8_t counter, ByteArray command) : _id(id), _command(command)
{
    _lead.init(&lead, 1);
    _counter.init(&counter, 1);
    _setCrc();
}

void IthoCommand::_setCrc()
{
    uint8_t s = 0 - sum();
    _crc.init(&s, 1);
}

String IthoCommand::toString()
{
    uint8_t crc = sum();

    String r = "i = ";
    r += _lead.toString() + ":";
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
