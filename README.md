# IthoRadio

This is a ESP8266 project that provides receiving and sending messages to Itho ventilation system. The start was based on
https://github.com/supersjimmie/IthoEcoFanRFT, but it ended up being a complete rewrite except for the CC1101 basis 
interaction. 

## Receiver example

The receiver was rewritten to handle variable length packets, which was neede for the more complicated commands.
The receiver can do some basic decoding of the messages. The begin and end are detected, and several de-whitening steps
are performed. Also a checksum is computed. This seems to decode message from several itho applications: remote controls, 
controller box and possibly 
communication between heatpumps. The receiver can currently only decode messages for the for the demand flow systems. 
When used to receive and decode all messages, other remotes can easily be added.

## Remote control example

This example implements a remote control replacement. A cc1101 and a ir receiver are hooked up to the ESP. The ir receiver is in the kitchen and receives commands form the AEG induction stove (hub2hood). These commands are used to turn on the cook mode of the Itho ventialtion unit.
