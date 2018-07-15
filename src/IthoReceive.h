#include <Arduino.h>

class IthoReceiveClass
{
  public:
	IthoReceiveClass() {}

	void setup();
	void loop();

	void setInterruptPin(uint8_t);
	void attachIter();
	void detachIter();
	void resetBuffer();

	bool printAllPacket = true;
	bool printNonRemote = true;
	bool printOtherRemote = true;
	
	bool printDebug = false;

  void logger(void (*)(const String &));

  private:
	uint8_t _irqPin = D2;
	bool _gotIthoStart;
	uint8_t _lastIndex;
	size_t _oldSize = 0;
	size_t _checkIdx = 0;

	static String toString(uint8_t *data, unsigned int length, bool ashex = true);


  void (*_logger)(const String &) = NULL;
  void _log(const String &s);
};

extern IthoReceiveClass IthoReceive;
