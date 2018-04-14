#include "DemandIthoCommand.h"

DemandIthoCommand::DemandIthoCommand(ByteArray id, uint8_t counter, ByteArray command) : 
    _id(id), _command(command)
{
    //Serial.println("DemandIthoCommand ctor");
    _counter.init(&counter, 1);
    _setCrc();
}

void DemandIthoCommand::_setCrc()
{
    uint8_t s = 234 - sum();
    _crc.init(&s, 1);
}