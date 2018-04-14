#include <inttypes.h>
#include <Arduino.h>

class ByteArray;

class BitArray {
    public:
        BitArray(unsigned int length, unsigned int maxLength = 0) {
            _length = length;
            _maxLength = (maxLength > 0) ? maxLength : length;
            _alength = wordIndex(_maxLength-1) + 1;
            _array = new uint8_t [_alength];
            for (unsigned int i = 0; i < _alength; i++)
                _array[i] = 0;
        };

        BitArray(String s) 
        {
            _length = s.length() * 8;
            _alength = wordIndex(_length) + 1;
            _array = new uint8_t [wordIndex(_length) + 1];
            for (unsigned int i = 0; i < _alength; i++)
                _array[i] = 0;
        }
        void set(unsigned int i, bool v, bool debug = false);

        bool get(unsigned int i) {
            if (i >= _length) {
                printf("Error, get out of range l=%d i=%d", _length, i);
            }
            unsigned int wi = wordIndex(i);
            uint8_t bi = bitIndex(i);
            return (_array[wi] >> bi) & 1L;
        }

        void append(const ByteArray& a);

        unsigned int length() const { return _length; }
        unsigned int alength() const { return _alength; }
        const uint8_t getWord(unsigned int i) const { return _array[i]; }

        void print() 
        {
            Serial.println(toString3());
        };

        String toString(uint8_t ts = 4) 
        {
            String r;
            for(unsigned int i = 0; i < _length; i++) {
                r += String(get(i) ? "1": "0");
                if (((i+1) % ts) == 0 && (i+1) < _length) {
                    r += ":";
                }
            }
            return r;
        };

        String toString2()
        {
            String r;
            for(unsigned int i = 0; i < (_alength); i++) {
                uint8_t cc = _array[i];
                printf("i = %d   cc = %d\n", i, cc);
                uint8_t c = 0;
                for (unsigned int j = 0; j < 8; j++) {
                    c += (cc >> (7 - j)) << j;
                }
                r += String(c, HEX) + ":";
                Serial.println(r);
            }
            return r;
        };

        String toString3()
        {
            String r;
            for(unsigned int i = 0; i < (_alength); i++) {
                uint8_t cc = _array[i];
                r += String(cc, HEX);
                if (i < _alength -1) {
                    r += ":";
                }
            }
            return r;
        };

        String toStringRaw()
        {
            String r;
            for (size_t i = 0; i < _alength; i++)
            {
                char c = _array[i];
                r += " ";
                r.setCharAt(r.length()-1, c);
            }
            return r;
        }

        unsigned int search(uint8_t a1, uint8_t a2) 
        {
            for(unsigned int i = 0; i < _alength-1; i++) {
                if(_array[i] == a1 && _array[i+1] == a2) {
                    _alength = i;
                    _length = 8 * _alength;
                    return i;
                }
            }
            return _alength;
        };

        uint8_t checksum() 
        {
            uint8_t c = 0;
            for (unsigned int i = 0; i < _alength; i++) {
                c += _array[i];
            }
            return c;
        };

        ~BitArray() {
            delete _array;
        }

    private:
        unsigned int _length;
        unsigned int _maxLength;
        unsigned int _alength;
        uint8_t * _array;

        unsigned int wordIndex(unsigned int i) {
            return i/8;
        }

        uint8_t bitIndex(unsigned int i) {
            return (7 - (i % 8));
        }
};
