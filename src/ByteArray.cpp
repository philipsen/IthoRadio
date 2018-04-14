#include "ByteArray.h"

#include "BitArray.h"

ByteArray::ByteArray(const BitArray &a)
{
    _data = 0;
    _init(a.alength());
    for (size_t i = 0; i < length(); i++)
    {
        _data[i] = a.getWord(i);
    }
}
