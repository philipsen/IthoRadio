#include "IthoSender.h"

#include "ByteArray.h"

#include "IthoCommand.h"
#include "IthoDecode.h"

uint8_t ecoBytes[] =     {0x22, 0xf8, 0x03, 0x00, 0x01, 0x02};
uint8_t comfortBytes[] = {0x22, 0xf8, 0x03, 0x00, 0x02, 0x02};
uint8_t cook1Bytes[] =   {0x22, 0xf3, 0x05, 0x00, 0x02, 0x1e, 0x02, 0x03};
uint8_t cook2Bytes[] =   {0x22, 0xf3, 0x05, 0x00, 0x02, 0x3c, 0x02, 0x03};
uint8_t timer1Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x03, 0x03, 0x03};
uint8_t timer2Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x06, 0x03, 0x03};
uint8_t timer3Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x09, 0x03, 0x03};
uint8_t joinBytes[] =    {0x1f, 0xc9, 0x0c, 0x00, 0x22, 0xf8, 0x52, 0x50, 0xb9, 0x00, 0x10, 0xe0, 0x52, 0x50, 0xb9};

void IthoSenderClass::remoteId(uint8_t* id)
{
    for (size_t i = 0; i < 3; i++)
    {
        _remoteId[i] = id[i];
    }
}


void IthoSenderClass::sendCommand(const String &c)
{
    Serial.print("send command: ");
    Serial.println(c);

    uint8_t* comBytes = 0;
    unsigned int comLength = 0;
    if (c == "eco")
    {
        digitalWrite(D0, 1);
        comBytes = ecoBytes;
        comLength = 6;
    }
    if (c == "cook1")
    {
        digitalWrite(D0, 0);
        comBytes = cook1Bytes;
        comLength = 8;
    }

    ByteArray id(_remoteId, 3);
    ByteArray cc(comBytes, comLength);
    IthoCommand cmd(id, _counter, cc);
    String ps = cmd.toString();
    Serial.print("send cmd: ");
    Serial.println(ps);

    ByteArray cmdEncoded = IthoDecode::encode(cmd);

    Serial.print("send encoded: ");
    Serial.println(cmdEncoded.toString());

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

IthoSenderClass IthoSender;