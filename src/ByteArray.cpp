#include "ByteArray.h"
#include "BitArray.h"
#include "StringSupport.h"
#include "IthoSender.h"

ByteArray::ByteArray(const BitArray &a)
{
    _data = 0;
    _init(a.alength());
    for (size_t i = 0; i < length(); i++)
    {
        _data[i] = a.getWord(i);
    }
}


ByteArray::ByteArray(const String& value)
{
    _data = 0;
    _size = 0;
    //IthoSender._log(String("ByteArray::ByteArray ") + value);
    int lengthBytes = count(':', value) + 1;
    //IthoSender._log(String("l=") + lengthBytes);
    uint8_t v[lengthBytes];
    for (int i = 0; i < lengthBytes; i++) {
        String ss = sub(value, ':', i);
        int bv = strToHex(ss);
        //IthoSender._log(String("val ") + i + "=" + ss + " " +bv);
        v[i] = bv;
    }
    init(v, lengthBytes);    
}
