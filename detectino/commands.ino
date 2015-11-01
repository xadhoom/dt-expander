#include "commands.h"

void read_from_bus() {
  unsigned long can_id;
  unsigned char len = 0;
  unsigned char data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    CAN.readMsgBufID(&can_id, &len, data);    // read data,  len: data length, buf: data buf

    Serial.print(F("Got message with CAN_ID :"));
    Serial.println(can_id);
    for (int i = 0; i < len; i++) {
      Serial.print(data[i]);
      Serial.print(F(" "));
    }
    Serial.println();
    dispatch_command(can_id, data);
  }
}

void dispatch_command(unsigned long can_id, unsigned char data[8]) {
  unsigned int command = (can_id >> 8) & 0xff;
  unsigned int subcommand = can_id & 0xff;
  unsigned int src_node_id = (can_id >> 23) & 0xff;

  switch (command) {
    case CMD_PING:
      handle_ping(src_node_id, data);
      break;
    case CMD_READ:
      handle_read(subcommand, src_node_id);
      break;
    case CMD_READD:
      handle_readd(subcommand, src_node_id);
      break;
    default:
      Serial.print(F("unknown command :"));
      Serial.println(command);
      break;
  }
}

void handle_read(unsigned int subcommand, unsigned int src_node_id) {
  Serial.println(F("Got analog read rq"));

  if (subcommand > NR_ANALOG_PINS)  {
    Serial.print(F("Unknown subcommand read : "));
    Serial.println(subcommand);
    return;
  }

  if (subcommand == 0) {
    /* read all */
    for (int i = 0; i < NR_ANALOG_PINS; i++) {
      CAN_Payload payload = init_sensor_payload(analog_values[i]);
      set_aterminal_id(analog_terminals_ids[i], &payload);
      send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_ALL, 8, payload);
    }
    return;
  }

  CAN_Payload payload = init_sensor_payload(analog_values[subcommand - 1]);
  set_aterminal_id(analog_terminals_ids[subcommand - 1], &payload);
  send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_ONE, 8, payload);
}

void handle_readd(unsigned int subcommand, unsigned int src_node_id) {
  Serial.println(F("Got digital read rq"));

  if (subcommand > NR_DIGITAL_PINS)  {
    Serial.print(F("Unknown subcommand digi read : "));
    Serial.println(subcommand);
    return;
  }

  if (subcommand == 0) {
    /* read all */
    for (int i = 0; i < NR_DIGITAL_PINS; i++) {
      CAN_Payload payload = init_sensor_payload(digital_values[i]);
      set_dterminal_id(digital_terminals_ids[i], &payload);
      send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_ALL, 8, payload);
    }
    return;
  }

  CAN_Payload payload = init_sensor_payload(digital_values[subcommand - 1]);
  set_dterminal_id(digital_terminals_ids[subcommand - 1], &payload);
  send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_ONE, 8, payload);
}

void handle_ping(unsigned int src_node_id, unsigned char data[8]) {
  Serial.println(F("Got ping rq"));
  CAN_Payload payload;
  memcpy(payload.data, data, 8);
  unsigned long pong = CMD_PONG << 8;
  send_data_to_node(src_node_id, pong, 8, payload);
}
