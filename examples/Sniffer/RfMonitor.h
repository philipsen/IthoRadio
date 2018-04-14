#include <Arduino.h>

class RfMonitor
{
  public:
    void setup();
    void loop();
    void attachIter();
    void detachIter();
    void resetBuffer();

    bool printAllPacket = false;
    bool printNonRemote = false;
    bool printDebug = false;

  private:
    bool _gotIthoStart;
    uint8_t _lastIndex;
    size_t oldSize = 0;
    size_t checkIdx = 0;
};
