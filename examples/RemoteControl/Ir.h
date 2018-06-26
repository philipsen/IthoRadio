#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IthoSender.h>

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
    //MqttCom.logger("ir loop");
    receiveIRCommand();
}

// Receive and decode IR commands and control hood upon received command
void receiveIRCommand()
{
    //MqttCom.logger("ir loop");
    // have we received an IR signal?
    if (irrecv.decode(&results))
    {

        int ventilation = 0;
        Serial.println("Received IR command: ");
        Serial.println((long)results.value, HEX);
        MqttCom.logger(String("received "));
        switch ((long)results.value)
        {
        case IRCMD_VENT_1:
            MqttCom.logger("turn on");
            ventilation = 1;
            IthoSender.turnOn();
            break;

        case IRCMD_VENT_2:
            ventilation = 2;
            IthoSender.turnOn();
            break;

        case IRCMD_VENT_3:
            ventilation = 3;
            IthoSender.turnOn();
            break;

        case IRCMD_VENT_4:
            ventilation = 4;
            IthoSender.turnOn();
            break;

        case IRCMD_VENT_OFF:
            IthoSender.turnOff();
            ventilation = 0;
            break;
        default:
            ventilation = 99;
            break;
        }
        printf("vent = %d\n", ventilation);

        irrecv.resume(); // receive the next value
    }
}
