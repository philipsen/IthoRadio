/*
 * Author: Wim
 * 
 * gebaseerd op werk van Klusjesman, modified bij supersjimmie for Arduino/ESP8266
 */

#ifndef __ITHOCC1101Class_H__
#define __ITHOCC1101Class_H__

#include <stdio.h>
#include "CC1101.h"

class ByteArray;

class IthoCC1101Class : public CC1101
{
	private:														
		
	public:
		IthoCC1101Class();
		~IthoCC1101Class();
		
		void setup();
		void init() { CC1101::init(); }
		void initReceive();

		void sendCommand(CC1101Packet outMessage);
		void initReceiveMessage();

	private:
		void initSendMessage(uint8_t pktLength);
		void finishTransfer();

}; 

extern IthoCC1101Class IthoCC1101;

#endif //__ITHOCC1101Class_H__
