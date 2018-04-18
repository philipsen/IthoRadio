/*
 * Author: Wim
 * 
 * gebaseerd op werk van Klusjesman, modified bij supersjimmie for Arduino/ESP8266
 */

#include "IthoCC1101.h"
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include "ByteArray.h"
#include "DemandIthoCommand.h"
#include "IthoDecode.h"

uint8_t ecoBytes[] =     {0x22, 0xf8, 0x03, 0x00, 0x01, 0x02};
uint8_t comfortBytes[] = {0x22, 0xf8, 0x03, 0x00, 0x02, 0x02};
uint8_t cook1Bytes[] =   {0x22, 0xf3, 0x05, 0x00, 0x02, 0x1e, 0x02, 0x03};
uint8_t cook2Bytes[] =   {0x22, 0xf3, 0x05, 0x00, 0x02, 0x3c, 0x02, 0x03};
uint8_t timer1Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x03, 0x03, 0x03};
uint8_t timer2Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x06, 0x03, 0x03};
uint8_t timer3Bytes[] =  {0x22, 0xf3, 0x05, 0x00, 0x42, 0x09, 0x03, 0x03};
uint8_t joinBytes[] =    {0x1f, 0xc9, 0x0c, 0x00, 0x22, 0xf8, 0x52, 0x50, 0xb9, 0x00, 0x10, 0xe0, 0x52, 0x50, 0xb9};



//pa table settings
const uint8_t ithoPaTableSend[8] = {0x6F, 0x26, 0x2E, 0x8C, 0x87, 0xCD, 0xC7, 0xC0};
const uint8_t ithoPaTableReceive[8] = {0x6F, 0x26, 0x2E, 0x7F, 0x8A, 0x84, 0xCA, 0xC4};

// default constructor
IthoCC1101Class::IthoCC1101Class() : CC1101()
{
} 

// default destructor
IthoCC1101Class::~IthoCC1101Class()
{
} 

void IthoCC1101Class::setup() 
{
    Serial.println("setup begin");
    IthoCC1101.init();
    IthoCC1101.initReceive();
    Serial.println("setup done");
}

void IthoCC1101Class::initSendMessage(uint8_t pktLength)
{
	//finishTransfer();
	writeCommand(CC1101_SIDLE);
	delayMicroseconds(1);
	writeRegister(CC1101_IOCFG0 ,0x2E);
	delayMicroseconds(1);
	writeRegister(CC1101_IOCFG1 ,0x2E);
	delayMicroseconds(1);
	writeCommand(CC1101_SIDLE);
	writeCommand(CC1101_SPWD);
	delayMicroseconds(2);
	
	/*
	Configuration reverse engineered from remote print. The commands below are used by IthoDaalderop.
		
	Base frequency		868.299866MHz
	Channel				0
	Channel spacing		199.951172kHz
	Carrier frequency	868.299866MHz
	Xtal frequency		26.000000MHz
	Data rate			38.3835kBaud
	Manchester			disabled
	Modulation			2-FSK
	Deviation			50.781250kHz
	TX power			?
	PA ramping			enabled
	Whitening			disabled
	*/	
	writeCommand(CC1101_SRES);
	delayMicroseconds(1);
	writeRegister(CC1101_IOCFG0 ,0x2E);		//High impedance (3-state)
	writeRegister(CC1101_FREQ2 ,0x21);		//00100001	878MHz-927.8MHz
	writeRegister(CC1101_FREQ1 ,0x65);		//01100101
	writeRegister(CC1101_FREQ0 ,0x6A);		//01101010	
	writeRegister(CC1101_MDMCFG4 ,0x5A);	//difference compared to message1
	writeRegister(CC1101_MDMCFG3 ,0x83);	//difference compared to message1
	writeRegister(CC1101_MDMCFG2 ,0x00);	//00000000	2-FSK, no manchester encoding/decoding, no preamble/sync
	writeRegister(CC1101_MDMCFG1 ,0x22);	//00100010
	writeRegister(CC1101_MDMCFG0 ,0xF8);	//11111000
	writeRegister(CC1101_CHANNR ,0x00);		//00000000
	writeRegister(CC1101_DEVIATN ,0x50);	//difference compared to message1
	writeRegister(CC1101_FREND0 ,0x17);		//00010111	use index 7 in PA table
	writeRegister(CC1101_MCSM0 ,0x18);		//00011000	PO timeout Approx. 146µs - 171µs, Auto calibrate When going from IDLE to RX or TX (or FSTXON)
	writeRegister(CC1101_FSCAL3 ,0xA9);		//10101001
	writeRegister(CC1101_FSCAL2 ,0x2A);		//00101010
	writeRegister(CC1101_FSCAL1 ,0x00);		//00000000
	writeRegister(CC1101_FSCAL0 ,0x11);		//00010001
	writeRegister(CC1101_FSTEST ,0x59);		//01011001	For test only. Do not write to this register.
	writeRegister(CC1101_TEST2 ,0x81);		//10000001	For test only. Do not write to this register.
	writeRegister(CC1101_TEST1 ,0x35);		//00110101	For test only. Do not write to this register.
	writeRegister(CC1101_TEST0 ,0x0B);		//00001011	For test only. Do not write to this register.
	writeRegister(CC1101_PKTCTRL0 ,0x12);	//00010010	Enable infinite length packets, CRC disabled, Turn data whitening off, Serial Synchronous mode
	writeRegister(CC1101_ADDR ,0x00);		//00000000
	writeRegister(CC1101_PKTLEN ,0xFF);		//11111111	//Not used, no hardware packet handling

	//0x6F,0x26,0x2E,0x8C,0x87,0xCD,0xC7,0xC0
	writeBurstRegister(CC1101_PATABLE | CC1101_WRITE_BURST, (uint8_t*)ithoPaTableSend, 8);

	//difference, message1 sends a STX here
	writeCommand(CC1101_SIDLE);
	writeCommand(CC1101_SIDLE);

	writeRegister(CC1101_MDMCFG4 ,0x5A);	//difference compared to message1
	writeRegister(CC1101_MDMCFG3 ,0x83);	//difference compared to message1
	writeRegister(CC1101_DEVIATN ,0x50);	//difference compared to message1
	writeRegister(CC1101_IOCFG0 ,0x2D);		//GDO0_Z_EN_N. When this output is 0, GDO0 is configured as input (for serial TX data).
	writeRegister(CC1101_IOCFG1 ,0x0B);		//Serial Clock. Synchronous to the data in synchronous serial mode.

	writeCommand(CC1101_STX);
	writeCommand(CC1101_SIDLE);

	writeRegister(CC1101_MDMCFG4 ,0x5A);	//difference compared to message1
	writeRegister(CC1101_MDMCFG3 ,0x83);	//difference compared to message1
	writeRegister(CC1101_DEVIATN ,0x50);	//difference compared to message1
	//writeRegister(CC1101_IOCFG0 ,0x2D);		//GDO0_Z_EN_N. When this output is 0, GDO0 is configured as input (for serial TX data).
	//writeRegister(CC1101_IOCFG1 ,0x0B);		//Serial Clock. Synchronous to the data in synchronous serial mode.

	//Itho is using serial mode for transmit. We want to use the TX FIFO with fixed packet length for simplicity.
	writeRegister(CC1101_IOCFG0 ,0x2E);
	writeRegister(CC1101_IOCFG1 ,0x2E);
	writeRegister(CC1101_PKTCTRL0 ,0x00);
	writeRegister(CC1101_PKTCTRL1 ,0x00);
	
	writeRegister(CC1101_PKTLEN , pktLength);

}

void IthoCC1101Class::finishTransfer()
{
	writeCommand(CC1101_SIDLE);
	delayMicroseconds(1);

	writeRegister(CC1101_IOCFG0 ,0x2E);
	writeRegister(CC1101_IOCFG1 ,0x2E);
	
	writeCommand(CC1101_SIDLE);
	writeCommand(CC1101_SPWD);
}

void IthoCC1101Class::initReceive()
{
	/*
	Configuration reverse engineered from RFT print.
	
	Base frequency		868.299866MHz
	Channel				0
	Channel spacing		199.951172kHz
	Carrier frequency	868.299866MHz
	Xtal frequency		26.000000MHz
	Data rate			38.3835kBaud
	RX filter BW		325.000000kHz
	Manchester			disabled
	Modulation			2-FSK
	Deviation			50.781250kHz
	TX power			0x6F,0x26,0x2E,0x7F,0x8A,0x84,0xCA,0xC4
	PA ramping			enabled
	Whitening			disabled
	*/	
	writeCommand(CC1101_SRES);

	writeRegister(CC1101_TEST0 ,0x09);
	writeRegister(CC1101_FSCAL2 ,0x00);
	
	//0x6F,0x26,0x2E,0x7F,0x8A,0x84,0xCA,0xC4
	writeBurstRegister(CC1101_PATABLE | CC1101_WRITE_BURST, (uint8_t*)ithoPaTableReceive, 8);
	
	writeCommand(CC1101_SCAL);

	//wait for calibration to finish
	while ((readRegisterWithSyncProblem(CC1101_MARCSTATE, CC1101_STATUS_REGISTER)) != CC1101_MARCSTATE_IDLE) yield();

	writeRegister(CC1101_FSCAL2 ,0x00);
	writeRegister(CC1101_MCSM0 ,0x18);			//no auto calibrate
	writeRegister(CC1101_FREQ2 ,0x21);
	writeRegister(CC1101_FREQ1 ,0x65);
	writeRegister(CC1101_FREQ0 ,0x6A);
	writeRegister(CC1101_IOCFG0 ,0x2E);			//High impedance (3-state)
	writeRegister(CC1101_IOCFG2 ,0x00);			//Assert when RX FIFO is filled or above the RX FIFO threshold. Deassert when (0x00): RX FIFO is drained below threshold, or (0x01): deassert when RX FIFO is empty.
	writeRegister(CC1101_FSCTRL1 ,0x06);
	writeRegister(CC1101_FSCTRL0 ,0x00);
	writeRegister(CC1101_MDMCFG4 ,0xE8);
	writeRegister(CC1101_MDMCFG3 ,0x43);
	writeRegister(CC1101_MDMCFG2 ,0x00);		//Enable digital DC blocking filter before demodulator, 2-FSK, Disable Manchester encoding/decoding, No preamble/sync 
	writeRegister(CC1101_MDMCFG1 ,0x22);		//Disable FEC
	writeRegister(CC1101_MDMCFG0 ,0xF8);
	writeRegister(CC1101_CHANNR ,0x00);
	writeRegister(CC1101_DEVIATN ,0x40);
	writeRegister(CC1101_FREND1 ,0x56);
	writeRegister(CC1101_FREND0 ,0x17);
	writeRegister(CC1101_MCSM0 ,0x18);			//no auto calibrate
	writeRegister(CC1101_FOCCFG ,0x16);
	writeRegister(CC1101_BSCFG ,0x6C);
	writeRegister(CC1101_AGCCTRL2 ,0x43);
	writeRegister(CC1101_AGCCTRL1 ,0x40);
	writeRegister(CC1101_AGCCTRL0 ,0x91);
	writeRegister(CC1101_FSCAL3 ,0xA9);
	writeRegister(CC1101_FSCAL2 ,0x2A);
	writeRegister(CC1101_FSCAL1 ,0x00);
	writeRegister(CC1101_FSCAL0 ,0x1F);
	writeRegister(CC1101_FSTEST ,0x59);
	writeRegister(CC1101_TEST2 ,0x81);
	writeRegister(CC1101_TEST1 ,0x35);
	writeRegister(CC1101_TEST0 ,0x0B);
	writeRegister(CC1101_PKTCTRL1 ,0x04);		//No address check, Append two bytes with status RSSI/LQI/CRC OK, 
	writeRegister(CC1101_PKTCTRL0 ,0x32);		//Infinite packet length mode, CRC disabled for TX and RX, No data whitening, Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins 
	writeRegister(CC1101_ADDR ,0x00);
	writeRegister(CC1101_PKTLEN ,0xFF);
	writeRegister(CC1101_TEST0 ,0x09);

	writeCommand(CC1101_SCAL);

	//wait for calibration to finish
	while ((readRegisterWithSyncProblem(CC1101_MARCSTATE, CC1101_STATUS_REGISTER)) != CC1101_MARCSTATE_IDLE) yield();

	writeRegister(CC1101_MCSM0 ,0x18);			//no auto calibrate
	
	writeCommand(CC1101_SIDLE);
	writeCommand(CC1101_SIDLE);
	
	writeRegister(CC1101_MDMCFG2 ,0x00);		//Enable digital DC blocking filter before demodulator, 2-FSK, Disable Manchester encoding/decoding, No preamble/sync 
	writeRegister(CC1101_IOCFG0 ,0x0D);			//Serial Data Output. Used for asynchronous serial mode.

	writeCommand(CC1101_SRX);
	
	while ((readRegisterWithSyncProblem(CC1101_MARCSTATE, CC1101_STATUS_REGISTER)) != CC1101_MARCSTATE_RX) yield();
	
	initReceiveMessage();
}

void  IthoCC1101Class::initReceiveMessage()
{
	uint8_t marcState;
	
	writeCommand(CC1101_SIDLE);	//idle
	
	//set datarate	
	writeRegister(CC1101_MDMCFG4 ,0x9A); // set kBaud
	writeRegister(CC1101_MDMCFG3 ,0x83); // set kBaud
	writeRegister(CC1101_DEVIATN ,0x50);
	
 	//set fifo mode with fixed packet length and sync bytes
	writeRegister(CC1101_PKTLEN, 0xff);			//42 bytes message (sync at beginning of message is removed by CC1101)
	//receiveState = ExpectNormalCommand;
	
	//set fifo mode with infinite packet length and sync bytes
	writeRegister(CC1101_PKTCTRL0 ,0x02);
	writeRegister(CC1101_SYNC1 ,170);			//message2 byte6
	writeRegister(CC1101_SYNC0 ,171);			//message2 byte7
	writeRegister(CC1101_MDMCFG2 ,0x02);
	writeRegister(CC1101_PKTCTRL1 ,0x00);	
	
	writeCommand(CC1101_SRX); //switch to RX state

	// Check that the RX state has been entered
	while (((marcState = readRegisterWithSyncProblem(CC1101_MARCSTATE, CC1101_STATUS_REGISTER)) & CC1101_BITS_MARCSTATE) != CC1101_MARCSTATE_RX)
	{
		if (marcState == CC1101_MARCSTATE_RXFIFO_OVERFLOW) // RX_OVERFLOW
			writeCommand(CC1101_SFRX); //flush RX buffer
	}
}

void IthoCC1101Class::sendCommand(CC1101Packet outMessage2)
{
	uint8_t maxTries = 3;
	uint8_t delaytime = 40;
	
	//send messages
	for (int i=0;i<maxTries;i++)
	{
		initSendMessage(outMessage2.length);
		sendData(&outMessage2);
		
		finishTransfer();
		delay(delaytime);
	}
    initReceive();
}

void IthoCC1101Class::sendCommand(const String &c)
{
    Serial.print("send command: ");
    Serial.println(c);

    uint8_t idBytes[] = {0x52, 0x50, 0xb9};

    uint8_t* comBytes = 0;
    unsigned int comLength = 0;
    if (c == "eco")
    {
        digitalWrite(D0, 1);
        comBytes = ecoBytes;
        comLength = 6;
    }
    if (c == "cook1")
    {
        digitalWrite(D0, 0);
        comBytes = cook1Bytes;
        comLength = 8;
    }

    ByteArray id(idBytes, 3);
    ByteArray cc(comBytes, comLength);
    DemandIthoCommand cmd(id, _counter, cc);
    String ps = cmd.toString();
    Serial.print("send cmd: ");
    Serial.println(ps);

    ByteArray cmdEncoded = IthoDecode::encode(cmd);

    Serial.print("send encoded: ");
    Serial.println(cmdEncoded.toString());

    CC1101Packet p;
    convertToPacket(cmdEncoded, p);
    IthoCC1101.sendCommand(p);

    _counter++;
}


void IthoCC1101Class::convertToPacket(const ByteArray &a, CC1101Packet &p)
{
    for (size_t i = 0; i < 7; i++)
        p.data[i] = 170;
    p.data[7] = 171;

    p.length = a.length() + 8;
    for (size_t i = 0; i < p.length; i++)
    {
        p.data[8 + i] = a.data(i);
    }
}

IthoCC1101Class IthoCC1101;
