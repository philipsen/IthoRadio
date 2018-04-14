#include "RfMonitor.h"
#include "IthoCC1101.h"
#include "IthoDecode.h"
#include "DemandIthoCommand.h"

#define ITHO_IRQ_PIN D2

#define LARGE_BUFFER_LEN 2052
#define LARGE_DATA_LEN CC1101_BUFFER_LEN - 3
uint8_t rfData[LARGE_BUFFER_LEN];
volatile unsigned int rfDataWriteIdx = 0;

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
    size_t rb = IthoCC1101.receiveDataRaw(rfData + rfDataWriteIdx, LARGE_BUFFER_LEN - rfDataWriteIdx);
    rfDataWriteIdx += rb;
    interruptCount++;
}

void RfMonitor::setup()
{
    Serial.println("setup begin");
    IthoCC1101.init();
    IthoCC1101.initReceive();
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



void RfMonitor::resetBuffer()
{
    detachIter();
    IthoCC1101.resetToReadState();
    checkIdx = 0;
    oldSize = 0;
    rfDataWriteIdx = 0;
    attachIter();
}
void RfMonitor::loop()
{
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
