/*
 * Author: Wim
 * 
 * gebaseerd op werk van Klusjesman, modified bij supersjimmie for Arduino/ESP8266
 */

#ifndef __ITHOCC1101_H__
#define __ITHOCC1101_H__

#include <stdio.h>
#include "CC1101.h"

class IthoCC1101 : public CC1101
{
	private:														
		
	//functions
	public:
		IthoCC1101();		//set initial counter value
		~IthoCC1101();
		
		//init
		void init() { CC1101::init(); }											//init,reset CC1101
		void initReceive();

		void sendCommand(CC1101Packet outMessage);
		void initReceiveMessage();

	private:
		void initSendMessage2(uint8_t pktLength);
		void finishTransfer();
}; 
#endif //__ITHOCC1101_H__
