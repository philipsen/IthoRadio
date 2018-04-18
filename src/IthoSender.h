#include <IthoCC1101.h>

class IthoSender
{
    public:
        static void turnOff() { IthoCC1101.sendCommand("eco"); }
        static void turnOn() { IthoCC1101.sendCommand("cook1"); }
};
