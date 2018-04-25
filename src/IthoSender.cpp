#include "IthoSender.h"

#include "ByteArray.h"

#include "IthoCommand.h"
#include "IthoDecode.h"

#include "RemoteCommand.h"


void IthoSenderClass::sendCommand(const String &c)
{
    Serial.print("send command: ");
    Serial.println(c);
    _log(String("send/") + c);
    const RemoteCommand* remoteCommand = _lookupByName(c, commands);
    if (remoteCommand == NULL) return;
    _send(_remoteId, remoteCommand);
}

void IthoSenderClass::sendCommandRoom(const String &c)
{
    Serial.print("send command room: ");
    Serial.println(c);
    _log(String("sendRoom/") + c);
    const RemoteCommand* remoteCommand = _lookupByName(c, commandsRoom);
    if (remoteCommand == NULL) return;
    _send(_remoteIdRoom, remoteCommand);
}

void IthoSenderClass::_send(uint8_t remoteId[], const RemoteCommand* remoteCommand)
{
    unsigned int comLength = 0;
    const uint8_t* comBytes = remoteCommand->bytes;
    comLength = remoteCommand->length;

    ByteArray id(remoteId, 3);
    ByteArray cc(comBytes, comLength);
    IthoCommand cmd(_remoteByte0, id, _counter, cc);
    String ps = cmd.toString();
    Serial.print("send cmd: ");
    Serial.println(ps);

    ByteArray cmdEncoded = IthoDecode::encode(cmd);

    //Serial.print("send encoded: ");
    //Serial.println(cmdEncoded.toString());
    CC1101Packet p;
    _convertToPacket(cmdEncoded, p);
    IthoCC1101.sendCommand(p);
    _counter++;
}

void IthoSenderClass::_convertToPacket(const ByteArray &a, CC1101Packet &p)
{
    for (size_t i = 0; i < 7; i++)
        p.data[i] = 170;
    p.data[7] = 171;

    p.length = a.length() + 8;
    for (size_t i = 0; i < p.length; i++)
    {
        p.data[8 + i] = a.data(i);
    }
}

void IthoSenderClass::remoteId(const uint8_t* id)
{
    for (size_t i = 0; i < 3; i++)
    {
        _remoteId[i] = id[i];
    }
}

void IthoSenderClass::remoteIdRoom(const uint8_t* id)
{
    for (size_t i = 0; i < 3; i++)
    {
        _remoteIdRoom[i] = id[i];
    }
}

void IthoSenderClass::logger(void (*callback) (const String&))
{
    _logger = callback;
}

void IthoSenderClass::_log(const String &s)
{
    if (_logger != NULL) {
        _logger(s);
    }
}

const RemoteCommand* IthoSenderClass::_lookupByName(const String& c, const RemoteCommand commands[])
{
    size_t i;
    for (i = 0; c != commands[i].name; i++)
    {
        if (commands[i].name == NULL) 
        {
            printf("unknown command %s\n", c.c_str());
            return NULL;
        }
    }

    if (c == "comfort" || c == "eco") {
        digitalWrite(D0, 1);
    } else {
        digitalWrite(D0, 0);
    }
    return &(commands[i]);
}

IthoSenderClass IthoSender;
