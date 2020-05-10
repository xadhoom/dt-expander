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
4:
5     : Analog Terminal ID
6, 7  : Used for sensors values (analog are 10bits, so we need 2 bytes)

*/
#define AT_BYTE 5 /* analog terminal id byte position */
#define VAL_BYTEMS 6 /* most significant value byte */
#define VAL_BYTELS 7 /* least significant value byte */
#define OC_PIN_BYTE 6 /* OC pin byte position */
#define RELAY_PIN_BYTE 6 /* OC pin byte position */

struct CAN_Payload {
  unsigned char data[8];
};

#define NR_ANALOG_PINS 16

/* list of IDs sent on can for each terminal */
const unsigned int analog_terminals_ids[NR_ANALOG_PINS] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

/*
 * PINs used for id (dip switch)
 */
#define ID_PIN1 22
#define ID_PIN2 23
#define ID_PIN3 24
#define ID_PIN4 25

/* PINS used for relays */
#define RELAYS_NR 2
#define RELAY1_PIN 32
#define RELAY2_PIN 33
const unsigned int relay_pins[RELAYS_NR] = {RELAY1_PIN, RELAY2_PIN};

/* PINS used for OC outputs */
#define OC_PINS_NR 6
#define OC1_PIN 26
#define OC2_PIN 27
#define OC3_PIN 28
#define OC4_PIN 29
#define OC5_PIN 30
#define OC6_PIN 31
const unsigned int oc_pins[OC_PINS_NR] = {OC1_PIN, OC2_PIN, OC3_PIN, OC4_PIN, OC5_PIN, OC6_PIN};


/*
 * Real Analog PINs for sensors input
 */
const unsigned int analog_terminals[NR_ANALOG_PINS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

/* Pin used for alive led */
#define ALIVE_PIN 2

/* how many retries for sending a frame over the canbus before giving up */
#define CAN_MAX_RETRY 5

/* CAN PINS */
#define CAN_CS_PIN 49

/* delta used to filter analog reads, each "unit" is little less than 5mV */
#define ANALOG_DELTA 5

#endif
