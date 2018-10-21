#include "IthoReceive.h"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include "IthoCC1101.h"
#include "IthoDecode.h"
#include "IthoCommand.h"

#define ITHO_IRQ_PIN D2
#define LARGE_BUFFER_LEN 2052
#define LARGE_DATA_LEN CC1101_BUFFER_LEN - 3
uint8_t rfData[LARGE_BUFFER_LEN];
volatile unsigned int rfDataWriteIdx = 0;

void IthoReceiveClass::setInterruptPin(uint8_t pin)
{
    _irqPin = pin;
    pinMode(_irqPin, OUTPUT);
}

String IthoReceiveClass::toString(uint8_t *data, unsigned int length, bool ashex)
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
    size_t rb = IthoCC1101.receiveDataRaw(rfData + rfDataWriteIdx, LARGE_BUFFER_LEN - rfDataWriteIdx);
    rfDataWriteIdx += rb;
}

void IthoReceiveClass::setup()
{
    pinMode(ITHO_IRQ_PIN, INPUT);
    attachIter();

    IthoCC1101.initReceive();
    _log("IthoReceiveClass::setup done");
}

void IthoReceiveClass::attachIter()
{
    attachInterrupt(ITHO_IRQ_PIN, ITHOinterrupt, RISING);
}

void IthoReceiveClass::detachIter()
{
    detachInterrupt(ITHO_IRQ_PIN);
}

void IthoReceiveClass::resetBuffer()
{
    detachIter();
    IthoCC1101.resetToReadState();
    _checkIdx = 0;
    _oldSize = 0;
    rfDataWriteIdx = 0;
    attachIter();
}
void IthoReceiveClass::loop()
{
    if (_oldSize != rfDataWriteIdx)
    {
        // check startbyte
        if (_oldSize == 0 && rfData[0] != 0xfe)
        {
            //printf("loop: drop packet b=%2x\n", rfData[0]);
            //Serial.println(toString(rfData, rfDataWriteIdx, true));
            resetBuffer();
            return;
        }

        if (rfDataWriteIdx > 1500)
        {
            if (printDebug)
                printf("loop: drop packet length %d\n", rfDataWriteIdx);
            rfDataWriteIdx = 0;
            //Serial.println(toString(rfData, rfDataWriteIdx, true));
            resetBuffer();
            return;
        }
        uint8_t preAmble[] = {0xfe, 0x00, 0xb3, 0x2a, 0xab, 0x2a};

        //printf("loop check for end seq at %d %d\n", checkIdx+1, rfDataWriteIdx);
        for (; _checkIdx + 2 < rfDataWriteIdx; _checkIdx++)
        {
            size_t i = _checkIdx;
            if ((rfData[i] == 0xac || rfData[i] == 0xca) &&
                rfData[i + 1] == 0xaa && rfData[i + 2] == 0xaa)
            {
                if (printAllPacket)
                {
                    //Serial.println("");
                    String s = toString(rfData, i + 3, true);
                    Serial.println(s);
                    _log(s);
                }
                bool isIthoRemote = true;
                for (size_t i = 0; i < 6; i++)
                {
                    if (rfData[i] != preAmble[i])
                    {
                        isIthoRemote = false;
                        break;
                    }
                }
                //printf("preamble check = %d\n", isIthoRemote);
                if (isIthoRemote)
                {
                    String s = IthoDecode::decode(rfData, i);
                    uint8_t crc = IthoDecode::crc(s);
                    if (s.charAt(0) == 0x16)
                    {
                        if (printOtherRemote)
                        {
                            Serial.printf("remote: ");
                            //String dc = IthoDecode::decode(rfData, i);
                            //Serial.println(IthoDecode::toPrintString(dc));
                            IthoCommand cmd(s);
                            Serial.println(cmd.toString());
                            _log(String("remote/") + IthoDecode::toPrintString(s));
                            _log(String("send/remote/") + cmd.id().toString() + "/" + cmd.command().toString());
                        }
                    }
                    else
                    {
                        if (printNonRemote)
                        {
                            String ss = String("non: ") + IthoDecode::toPrintString(s);
                            Serial.printf("other (crc=%d): ", crc);
                            Serial.println(ss);
                            //_log("got non remote");
                            _log(ss);
                        }
                    }
                }

                resetBuffer();
                return;
            }
        }
        _oldSize = rfDataWriteIdx;
    }
}

void IthoReceiveClass::logger(void (*callback) (const String&))
{
    _logger = callback;
}

void IthoReceiveClass::_log(const String &s)
{
    if (_logger != NULL) {
        _logger(s);
    }
}


IthoReceiveClass IthoReceive;
