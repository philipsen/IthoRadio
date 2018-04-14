#include "RfRemote.h"

#include <SPI.h>
#include <ESP8266WiFi.h>

#include "IthoCC1101.h"

#include "IthoDecode.h"
#include "DemandIthoCommand.h"

#define ITHO_IRQ_PIN D2

#define LARGE_BUFFER_LEN 2052
#define LARGE_DATA_LEN CC1101_BUFFER_LEN - 3
uint8_t rfData[LARGE_BUFFER_LEN];
volatile unsigned int rfDataWriteIdx = 0;

IthoCC1101 cc1101;

size_t interruptCount = 0;

String toString(uint8_t *data, unsigned int length, bool ashex = true)
{
    String str = "";
    for (uint8_t i = 0; i < length; i++)
    {
        if (ashex)
        {
            if (data[i] == 0)
            {
                str += "0";
            }
            str += String(data[i], HEX);
        }
        else
        {
            str += String(data[i]);
        }
        if (i < length - 1)
            str += ":";
    }
    return str;
}

void ITHOinterrupt()
{
    size_t rb = cc1101.receiveDataRaw(rfData + rfDataWriteIdx, LARGE_BUFFER_LEN - rfDataWriteIdx);
    rfDataWriteIdx += rb;
    interruptCount++;
}

void RfRemote::setup()
{
    Serial.println("setup begin");
    cc1101.init();
    cc1101.initReceive();
    Serial.println("setup done");
    pinMode(ITHO_IRQ_PIN, INPUT);
    attachIter();
}

void RfRemote::attachIter()
{
    attachInterrupt(ITHO_IRQ_PIN, ITHOinterrupt, RISING);
}

void RfRemote::detachIter()
{
    detachInterrupt(ITHO_IRQ_PIN);
}

size_t oldSize = 0;
int loopCount = 0;
size_t checkIdx = 0;

void RfRemote::resetBuffer()
{
    detachIter();
    cc1101.resetToReadState();
    checkIdx = 0;
    oldSize = 0;
    rfDataWriteIdx = 0;
    attachIter();
}
void RfRemote::loop()
{
    loopCount++;
    if (printDebug && loopCount % 10000000 == 0)
        printf("ran loop %d times %d\n", loopCount, interruptCount);

    if (oldSize != rfDataWriteIdx)
    {
        // check startbyte
        if (oldSize == 0 && rfData[0] != 0xfe)
        {
            //printf("loop: drop packet b=%2x\n", rfData[0]);
            //Serial.println(toString(rfData, rfDataWriteIdx, true));
            resetBuffer();
            return;
        }

        if (rfDataWriteIdx > 1500)
        {
            if (printDebug) printf("loop: drop packet length %d\n", rfDataWriteIdx);
            rfDataWriteIdx = 0;
            //Serial.println(toString(rfData, rfDataWriteIdx, true));
            resetBuffer();
            return;
        }
        uint8_t preAmble[] = {0xfe, 0x00, 0xb3, 0x2a, 0xab, 0x2a};  

        //printf("loop check for end seq at %d %d\n", checkIdx+1, rfDataWriteIdx);
        for (; checkIdx+2 < rfDataWriteIdx; checkIdx++)
        {
            size_t i = checkIdx;
            if ((rfData[i] == 0xac || rfData[i] == 0xca) &&
                rfData[i + 1] == 0xaa && rfData[i + 2] == 0xaa)
            {
                if (printAllPacket) 
                {
                    //Serial.println("");
                    Serial.println(toString(rfData, i+3, true));
                }
                bool isIthoRemote = true;
                for (size_t i = 0; i < 6; i++)
                {
                    if(rfData[i] != preAmble[i]) {
                        isIthoRemote = false;
                        break;
                    }
                }
                //printf("preamble check = %d\n", isIthoRemote);
                if (isIthoRemote)
                {
                    String s = IthoDecode::decode2(rfData, i);
                    uint8_t crc = IthoDecode::crc(s);
                    //Serial.println(crc);
                    //printf("crc val = %d\n", crc);
                    if (s.charAt(0) == 0x16)
                    {
                        Serial.printf("remote: ");
                        String dc = IthoDecode::decode(rfData, i);
                        //Serial.println(IthoDecode::toPrintString(dc));
                        DemandIthoCommand cmd(s);
                        Serial.println(cmd.toString());
                    }
                    else
                    {
                        if (printNonRemote) {
                        Serial.printf("other (crc=%d): ", crc);
                        Serial.println(IthoDecode::toPrintString(s));
                        }
                    }
                }
                
                //printf("got packet l=%d, crc=%d p=\n", i+3, crc);
                // 

                // Serial.println(isIthoRemote);
                // if (isIthoRemote) {
                //     
                // }
                resetBuffer();
                return;
            }
        }
        oldSize = rfDataWriteIdx;
    }
}


uint8_t ecoBytes[] =     {0x22, 0xf8, 0x03, 0x00, 0x01, 0x02};
uint8_t comfortBytes[] = {0x22, 0xf8, 0x03, 0x00, 0x02, 0x02};
uint8_t cook1Bytes[] =   {0x22, 0xf3, 0x05, 0x00, 0x02, 0x1e, 0x02, 0x03};
uint8_t cook2Bytes[] =   {0x22, 0xf3, 0x05, 0x00, 0x02, 0x3c, 0x02, 0x03};
uint8_t timer1Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x03, 0x03, 0x03};
uint8_t timer2Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x06, 0x03, 0x03};
uint8_t timer3Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x09, 0x03, 0x03};
uint8_t joinBytes[] =    {0x1f, 0xc9, 0x0c, 0x00, 0x22, 0xf8, 0x52, 0x50, 0xb9, 0x00, 0x10, 0xe0, 0x52, 0x50, 0xb9};


void RfRemote::turnOn()
{
    sendCommand("cook1");
}
void RfRemote::turnOff()
{
    sendCommand("eco");
}

void RfRemote::sendCommand(const String &c)
{
    Serial.print("send command: ");
    Serial.println(c);

    uint8_t idBytes[] = {0x52, 0x50, 0xb9};

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

    ByteArray id(idBytes, 3);
    ByteArray cc(comBytes, comLength);
    DemandIthoCommand cmd(id, _counter, cc);
    String ps = cmd.toString();
    Serial.print("send cmd: ");
    Serial.println(ps);

    ByteArray cmdEncoded = IthoDecode::encode(cmd);

    Serial.print("send encoded: ");
    Serial.println(cmdEncoded.toString());

    CC1101Packet p;
    convertToPacket(cmdEncoded, p);
    cc1101.sendCommand(p);

    _counter++;
}

void RfRemote::convertToPacket(const ByteArray &a, CC1101Packet &p)
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

RfRemote* RfRemote::_globalRf = 0;