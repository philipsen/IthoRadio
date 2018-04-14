#include "BitArray.h"
#include "ByteArray.h"

void BitArray::set(unsigned int i, bool v, bool debug)
{
    if (i >= _maxLength) {
        printf("BitArray::set idx out of range %d >= %d\n", i, _maxLength);
        return;
    }
    if (debug) printf("BitArray::set %d, %d\n", i, v);
    unsigned int wi = wordIndex(i);
    uint8_t bi = bitIndex(i);
    if (v) {
        _array[wi] = _array[wi] | (1L << bi); 
    } else {
        _array[wi] = _array[wi] & (~(1L << bi)); 
    }
    if (i >= _length) { _length = i + 1; }
}

void BitArray::append(const ByteArray &a)
{
    for (size_t i = 0; i < a.length(); i++)
    {
        uint8_t b = a.data(i);
        //printf("b = %x\n", b);
        for (size_t j = 0; j < 8; j++)
        {
            set(length(), (b >> (7 - j) & 1L));
        }
    }
}
