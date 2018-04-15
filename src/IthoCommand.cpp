#include "IthoCommand.h"

IthoCommand::IthoCommand(uint8_t lead, ByteArray id, uint8_t counter, ByteArray command) : 
    _id(id), _command(command)
{
    _lead.init(&lead, 1);
    _counter.init(&counter, 1);
    _setCrc();
}

void IthoCommand::_setCrc()
{
    uint8_t s = 234 - sum();
    _crc.init(&s, 1);
}