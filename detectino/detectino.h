#ifndef __DETECTINO_H__
#define __DETECTINO_H__
/*
CAN BUS is used with extended frame format. This allows 29 bit identifiers.
We use the 7 MSBs as node id, even if the board as only a 4 bit dip, to
leave space to improvement.

Please note that we don't use message ids to handle also priority,
where in can bus lower message id has higher priority, since we don't need it.
I prefered (and need) to build the id to be able to address each node
in order to get where it is and what it has attached to it.

Special nodes id:
0x0 : the master node (raspberry in detectino project).
0x7f: the broadcast address

The other bits are used to identify destination and commands.

so here's the bit map:
id

23-29 : node id (source, read from dips)
16-22 : destination node id
 8-15 : command
 0- 7 : subcommand

*/

#define CAN_DST_MASK 0x7F0000 // mask to enable filter on destination node
#define CAN_BCAST_FILT 0x7F0000 // 0x7f on bits 16-22
#define CAN_BCAST 0x7F // the bcast addr
#define CAN_MASTER 0x0 // the master node addr, raspberry module in this case

/*
CAN BUS has 8 bytes payload. Each byte is described below.

0:
1:
2:
3:
4     : Digital Terminal ID
5     : Analog Terminal ID
6, 7  : Used for sensors values (analog are 10bits, so we need 2 bytes)

*/
#define DT_BYTE 4 /* digital terminal id byte position */
#define AT_BYTE 5 /* analog terminal id byte position */
#define VAL_BYTEMS 6 /* most significant value byte */
#define VAL_BYTELS 7 /* least significant value byte */

struct CAN_Payload {
  unsigned char data[8];
};

#define NR_DIGITAL_PINS 3
#define NR_ANALOG_PINS 8

/* list of IDs sent on can for each terminal */
const unsigned int analog_terminals_ids[NR_ANALOG_PINS] = {1, 2, 3, 4, 5, 6, 7, 8};
const unsigned int digital_terminals_ids[NR_DIGITAL_PINS] = {1, 2, 3};

/*
 * PINs used for id (dip switch)
 */
#define ID_PIN1 5
#define ID_PIN2 4
#define ID_PIN3 3
#define ID_PIN4 2

/*
 * Real PINs used for OC/DI inputs
 */
const unsigned int digital_terminals[NR_DIGITAL_PINS] = {6, 7, 8};

/*
 * Real Analog PINs for sensors input, inverted since the silk on pcb is wrong :)
 */
const unsigned int analog_terminals[NR_ANALOG_PINS] = {7, 6, 5, 4, 3, 2, 1, 0};

/* Pin used for alive led */
#define ALIVE_PIN 9

/* how many retries for sending a frame over the canbus before giving up */
#define CAN_MAX_RETRY 5

/* delta used to filter analog reads, each "unit" is little less than 5mV */
#define ANALOG_DELTA 5

#endif
