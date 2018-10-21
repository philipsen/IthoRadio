
inline int count(const char c, const String& s)
{
    int r = 0;
    for (unsigned int i = 0; i < s.length(); i++) {
        if (s.charAt(i) == c) r++;
    }
    return r;
}

inline String sub(const String & s, char separator, int cnt) 
{
    int startIndex = 0;
    for (int i = 0; i < cnt; i++) {
        startIndex = s.indexOf(separator, startIndex);
        if (startIndex < 0) return "";
        startIndex++;
    }
    int lastIndex = s.indexOf(separator, startIndex);
    if (lastIndex < 0) {
        return s.substring(startIndex);
    }
    return s.substring(startIndex, lastIndex);
}


inline int strToHex(const String &s)
{
  return (int) strtol(s.c_str(), 0, 16);
}
