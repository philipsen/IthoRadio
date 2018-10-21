#ifndef __ByteArray__
#define __ByteArray__

#include <inttypes.h>
#include <Arduino.h>

class BitArray;

class ByteArray
{
    private:

        unsigned int _size;
        uint8_t * _data;

    public:

    ByteArray()
    {
        _size = 0;
        _data = 0;
    }

    ByteArray(const ByteArray& a) 
    {
        _data = 0;
        _init(a.length());
        for (size_t i = 0; i < length(); i++)
        {
            _data[i] = a.data(i);
        }
    }

    ByteArray(const uint8_t* d, unsigned int s) 
    {
        _data = 0;
        _init(s);

        for (unsigned int i = 0; i < s; i++)  
        {
            _data[i] = d[i];
        }
    }

    ByteArray(const BitArray&);
    ByteArray(const String& value);

    unsigned int length() const { return _size; }

    uint8_t data(unsigned int i) const
    {
        return _data[i];
    }

    ~ByteArray()
    {
        _erase();
    }

    void init(uint8_t* d, unsigned int s) 
    {
        _init(s);
        for (size_t i = 0; i < s; i++)  
        {
            _data[i] = d[i];
        }
    }
    void substring(const String &s, size_t f, size_t t)
    {
        _init(t - f);
        for (size_t i = 0; i < _size; i++)
        {
            _data[i] = s.charAt(i + f);
        }
    }

    uint8_t sum() 
    {
        uint8_t r = 0;
        for (size_t i = 0; i < _size; i++)
        {
            r += _data[i];
        }
        return r;         
    }

    String toString() const
    {
        String r;
        for (size_t i = 0; i < _size; i++)
        {
            r += String(_data[i], HEX) + ":";
        }
        r.remove(r.length()-1);
        return r;
    }


  private:
    void _erase()
    {
        if (_data)
            delete _data;
        _data = 0;
        _size = 0;
    }
        
    void _init(size_t size)
    {
        //static_assert(size >= 0);
        _erase();
        _size = size;
        _data = new uint8_t[_size];
    }

};


#endif