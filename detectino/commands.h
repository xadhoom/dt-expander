#ifndef __COMMANDS_H__
#define __COMMANDS_H__

/* we use 16 bit to represent commands and subcommands.
 * This gives us space to 65535 combinations that are more than enough.
 *
 * list of commands, no more than 255
 */

enum commands {
  CMD_PING = 1,
  CMD_PONG = 2,
  CMD_EVENT = 3,
  CMD_READ = 4, /* analog read */
  CMD_READD = 5, /* digital read */
  /* put here more commands */
  CMD_END = 255
};

/* list of subcommands, each command has it own subcommands */
// DT_EVENT
enum subcommand_event {
  SC_EVENT_USC = 0, /* unsolicited */
  SC_EVENT_REPLY = 1, /* event as reply to some read request */
  SC_EVENT_READ_ALL = 2,
  SC_EVENT_READ_ONE = 4,
};

// DT_READ
enum subcommand_read {
  SC_READ_ALL = 0,
  SC_READ_T1 = 1,
  SC_READ_T2 = 2,
  SC_READ_T3 = 3,
  SC_READ_T4 = 4,
  SC_READ_T5 = 5,
  SC_READ_T6 = 6,
  SC_READ_T7 = 7,
  SC_READ_T8 = 8,
  SC_READ_T9 = 9,
  SC_READ_T10 = 10,
  SC_READ_T11 = 11,
  SC_READ_T12 = 12,
  SC_READ_T13 = 13,
  SC_READ_T14 = 14,
  SC_READ_T15 = 15,
  SC_READ_T16 = 16,
  SC_READ_END = 255
};

#endif

