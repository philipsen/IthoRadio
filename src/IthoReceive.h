#include <Arduino.h>

class IthoReceiveClass
{
  public:
	IthoReceiveClass() {}

	void setup();
	void loop();

	void attachIter();
	void detachIter();
	void resetBuffer();

	bool printAllPacket = true;
	bool printNonRemote = true;
	bool printDebug = false;

  private:
	bool _gotIthoStart;
	uint8_t _lastIndex;
	size_t _oldSize = 0;
	size_t _checkIdx = 0;

	static String toString(uint8_t *data, unsigned int length, bool ashex = true);
};

extern IthoReceiveClass IthoReceive;
