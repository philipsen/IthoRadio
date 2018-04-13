#include "RfMonitor.h"

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

void RfMonitor::setup()
{
    Serial.println("setup begin");
    cc1101.init();
    cc1101.initReceive();
    Serial.println("setup done");
    pinMode(ITHO_IRQ_PIN, INPUT);
    attachIter();
}

void RfMonitor::attachIter()
{
    attachInterrupt(ITHO_IRQ_PIN, ITHOinterrupt, RISING);
}

void RfMonitor::detachIter()
{
    detachInterrupt(ITHO_IRQ_PIN);
}

size_t oldSize = 0;
int loopCount = 0;
size_t checkIdx = 0;

void RfMonitor::resetBuffer()
{
    detachIter();
    cc1101.resetToReadState();
    checkIdx = 0;
    oldSize = 0;
    rfDataWriteIdx = 0;
    attachIter();
}
void RfMonitor::loop()
{
    loopCount++;
    if (loopCount % 10000000 == 0)
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
            printf("loop: drop packet length %d\n", rfDataWriteIdx);
            rfDataWriteIdx = 0;
            //Serial.println(toString(rfData, rfDataWriteIdx, true));
            resetBuffer();
            return;
        }

        //printf("loop check for end seq at %d %d\n", checkIdx+1, rfDataWriteIdx);
        for (; checkIdx+2 < rfDataWriteIdx; checkIdx++)
        {
            size_t i = checkIdx;
            if ((rfData[i] == 0xac || rfData[i] == 0xca) &&
                rfData[i + 1] == 0xaa && rfData[i + 2] == 0xaa)
            {
                //printf("loop: rfDataWriteIdx %d -> %d\n", oldSize, rfDataWriteIdx);
                //printf("loop: detect end seq at %d\n", i);
                Serial.println(toString(rfData, i+3, true));
                IthoDecode::decode2(rfData, i+3);

                resetBuffer();
                return;
            }
        }
        oldSize = rfDataWriteIdx;
    }
}