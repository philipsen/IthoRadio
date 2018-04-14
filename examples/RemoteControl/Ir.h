#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

void receiveIRCommand();

// IR commands from AEG hob2hood device
const long IRCMD_VENT_1 = 0xE3C01BE2;
const long IRCMD_VENT_2 = 0xD051C301;
const long IRCMD_VENT_3 = 0xC22FFFD7;
const long IRCMD_VENT_4 = 0xB9121B29;
const long IRCMD_VENT_OFF = 0x55303A3;

// IR Receiver PIN
const int PIN_IR_RECEIVER = 5;  // use D1
IRrecv irrecv(PIN_IR_RECEIVER); // create instance of 'irrecv'
decode_results results;

void setupIr()
{
    pinMode(D0, OUTPUT);
    pinMode(D4, OUTPUT);
    digitalWrite(D0, 0);
    digitalWrite(D4, 0);

    irrecv.enableIRIn(); // Start the receiver
    digitalWrite(D0, 1);
    digitalWrite(D4, 1);
}

void loopIr()
{
    receiveIRCommand();
}

// Receive and decode IR commands and control hood upon received command
void receiveIRCommand()
{
    // have we received an IR signal?
    if (irrecv.decode(&results))
    {
        int ventilation = 0;
        Serial.println("Received IR command: ");
        Serial.println((long)results.value, HEX);

        switch ((long)results.value)
        {
        case IRCMD_VENT_1:
            ventilation = 1;
            RfRemote.turnOn();
            break;

        case IRCMD_VENT_2:
            ventilation = 2;
            RfRemote.turnOn();
            break;

        case IRCMD_VENT_3:
            ventilation = 3;
            RfRemote.turnOn();
            break;

        case IRCMD_VENT_4:
            ventilation = 4;
            RfRemote.turnOn();
            break;

        case IRCMD_VENT_OFF:
            RfRemote.turnOff();
            ventilation = 0;
            break;
        default:
            ventilation = 99;
            break;
        }
        printf("vent = %d\n", ventilation);

        // if (ventilation != 199)
        // {
        //     //mqtt.publish("hubirremote", String(ventilation).c_str());
        // }
        // transmit received message
        //send(hoodMessage.set(ventilation));
        //String s = String(results.value, HEX).c_str();
        //Serial.println("here");
        //Serial.println(s); // display it on serial monitor in hexadecimal
        //send(irMessage.set(s.c_str()));
        irrecv.resume(); // receive the next value
    }
}
