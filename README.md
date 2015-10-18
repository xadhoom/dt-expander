### Arduino code for detectino alarm board.

This code is meant to be run on Detectino Alarm hardware.
Schematics can be found on https://github.com/xadhoom/dt-expander-hardware .

The purpose of this code is to provide A/D inputs
readings over the CAN bus.

Since is an alarm system, right now the code provides different functions:

* periodic readings of all values (10 seconds by default)
* notify a value when it changes (for example, when an alarm sensor is triggered)
* answer to "ping" requests (useful to check is the node is reachable over CAN bus)

The code tries to filter out spikes caused by external factors,
keeping in mind that all commercial alarm sensors keep it's outputs
for a long time.

CAN bus is used only as physical transport, no special protocol is present.

CAN bus message ID (using 29 bits of extended) is used to carry 
source address, destination address, command and subcommand.

The payload is used to carry readings and analog or digital pins ids.

Refer to source code for further details.

