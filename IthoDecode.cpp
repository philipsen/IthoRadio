#include "IthoDecode.h"
#include "DemandIthoCommand.h"

#define DEBUG 0

String IthoDecode::toPrintString(const String &s)
{
    String r;
    for (size_t i = 0; i < s.length(); i++)
    {
        char c = s.charAt(i);
        //printf("tps: %d %2x %c\n", i, c, isPrintable(c) ? c : ' ');
        r += String(s.charAt(i), HEX);
        if ((i + 1) < s.length())
        {
            r += ":";
        }
    }
    return r;
}

uint8_t IthoDecode::crc(const String &rawByteString)
{
    uint8_t r = 0;
    for (size_t i = 0; i < rawByteString.length(); i++)
    {
        char c = rawByteString.charAt(i);
        r += (uint8_t)c;
    }
    return r;
}

String IthoDecode::decode(uint8_t *data, uint8_t length)
{
    for (int i = 0; i < 8; i++)
    {
        if (data[i] != _preamble[i])
        {
            return String("");
        }
    }
    data = data + 8;
    length -= 8;
    //length = 4;
    unsigned int l2 = length * 8;
    //printf("dec %d, %d\n", length, l2);
    BitArray b(l2);
    String s = b.toString();
    for (unsigned int j = 0; j < length; j++)
    {
        uint8_t c = data[j];
        //printf("byte= %d\tc = %x\n", j, c);
        for (uint8_t i = 0; i < 8; i++)
        {
            unsigned int idx = (j * 8) + (7 - i);
            bool v = (c >> i) & 1;
            //printf("c2 = %x\n", c >> i);
            //printf("i = %d\tv=%d\n", idx, v);
            b.set(idx, v);
            //Serial.println(b.toString());
        }
        //printf("\n\n");
        //Serial.println(b.toString2());
    }
    //printf("After preamble:\n");
    //Serial.println(b.toString());

    // first 4 and drop odd bits
        printf("here2\n");

    BitArray b2((l2 - 4) / 2);
    for (unsigned int i = 0; i < l2 / 2; i++)
    {
        b2.set(i, b.get(4 + (i * 2)));
    }
    //printf("After drop bits:\n");
    //Serial.println(b2.toString());

    // div into group of 5, drop last bit and reverse
    unsigned int nf = (b2.length() / 5);
    unsigned int fl = nf * 4;
    printf("here3\n");
    BitArray b3(fl);
    for (unsigned int i = 0; i < nf; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            b3.set(i * 4 + j, b2.get(i * 5 + (3 - j)));
        }
    }
    //printf("After octet coding:\n");
    //Serial.println(b3.toString());
    //Serial.println(b3.toString3());

    uint8_t l = b3.search(0x7f, 0xff);
    //Serial.println(b3.toString3());
    //printf("crc = %d\n", b3.checksum());

    String rs = b3.toStringRaw();
    //String ps = toPrintString(rs);

    //Serial.print("str convert :");
    //Serial.println(ps);
    return rs;
}
String IthoDecode::decode2(uint8_t *data, uint8_t length)
{
    data = data + 6;
    length -= 6;
    //length = 4;
    unsigned int l2 = length * 8;
    //printf("dec %d, %d\n", length, l2);
    BitArray b(l2);
    String s = b.toString();
    for (unsigned int j = 0; j < length; j++)
    {
        uint8_t c = data[j];
        //printf("byte= %d\tc = %x\n", j, c);
        for (uint8_t i = 0; i < 8; i++)
        {
            unsigned int idx = (j * 8) + (7 - i);
            bool v = (c >> i) & 1;
            //printf("c2 = %x\n", c >> i);
            //printf("i = %d\tv=%d\n", idx, v);
            b.set(idx, v);
            //Serial.println(b.toString());
        }
        //printf("\n\n");
        //Serial.println(b.toString2());
    }

    // drop odd bits
    BitArray b2((l2) / 2);
    for (unsigned int i = 0; i < l2 / 2; i++)
    {
        b2.set(i, b.get((i * 2)));
    }

    // div into group of 5, drop last bit and reverse
    unsigned int nf = (b2.length() / 5);
    unsigned int fl = nf * 4;
    BitArray b3(fl);
    for (unsigned int i = 0; i < nf; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            b3.set(i * 4 + j, b2.get(i * 5 + (3 - j)));
        }
    }
    
    String rs = b3.toStringRaw();
    //String ps = toPrintString(rs);
    //Serial.print("str convert :");
    //Serial.println(ps);
    return rs;
}

ByteArray IthoDecode::encode(DemandIthoCommand &cmd)
{
    unsigned int cmdLength = cmd.id().length() + 1 + cmd.command().length() + 1;
    Serial.printf("IthoDecode::encode cmd=%s  l=%d\n", cmd.toString().c_str(), cmdLength);
    BitArray tmp(0, cmdLength * 8);
    tmp.append(cmd.id());
    tmp.append(cmd.counter());
    tmp.append(cmd.command());
    tmp.append(cmd.crc());

    if (DEBUG)
    {
        printf("After concat\n");
        tmp.print();
        Serial.println(tmp.toString(4));
    }

    size_t numOct = (cmdLength * 8) / 4;
    BitArray tmp2(2 + (numOct * 5));
    tmp2.set(0, false);
    tmp2.set(1, true);
    for (size_t i = 0; i < numOct; i++)
    {
        // use a half byte, reverse
        for (size_t j = 0; j < 4; j++)
        {
            tmp2.set(2 + (5 * i) + (3 - j), tmp.get((4 * i) + j));
        }
        // set and extra bit to 1 to complete the octet
        tmp2.set(2 + (5 * i) + 4, true);
    }

    if (DEBUG)
    {
        printf("After de oct reverse (n=%d, l=%d)\n", numOct, tmp2.length());
        //tmp2.print();
        Serial.println(tmp2.toString(4));
    }
    BitArray tmp3(0, (8 * 8) + (tmp2.length() * 2) + (16 * 8));
    tmp3.append(ByteArray(_preamble, 8));
    for (size_t i = 0; i < tmp2.length(); i++)
    {
        tmp3.set((8 * 8) + (i * 2), tmp2.get(i));
        tmp3.set((8 * 8) + (i * 2) + 1, !tmp2.get(i));
    }

    tmp3.append(ByteArray(_postamble, 16));
    if (DEBUG)
    {
        printf("After bit white (l=%d)\n", tmp3.length());
        tmp3.print();
    }
    return ByteArray(tmp3);
}

const uint8_t IthoDecode::_preamble[] = {0xfe, 0x00, 0xb3, 0x2a, 0xab, 0x2a, 0x95, 0x9a};
const uint8_t IthoDecode::_postamble[] = {0xac, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};