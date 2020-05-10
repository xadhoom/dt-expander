#include "commands.h"

void read_from_bus() {
  unsigned long can_id = 0;
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
    case CMD_READ_ANALOG:
      handle_analog_read(subcommand, src_node_id);
      break;
    case CMD_READ_RELAY:
      handle_relay_read(subcommand, src_node_id);
      break;
    case CMD_READ_OC:
      handle_oc_read(subcommand, src_node_id);
      break;
    case CMD_SWITCH_RELAY_ON:
      handle_switch_relay(subcommand, src_node_id, true);
      break;
    case CMD_SWITCH_RELAY_OFF:
      handle_switch_relay(subcommand, src_node_id, false);
      break;
    case CMD_SWITCH_OC_ON:
      handle_switch_oc(subcommand, src_node_id, true);
      break;
    case CMD_SWITCH_OC_OFF:
      handle_switch_oc(subcommand, src_node_id, false);
      break;
    default:
      Serial.print(F("unknown command :"));
      Serial.println(command);
      break;
  }
}

void handle_switch_relay(unsigned int subcommand, unsigned int src_node_id, boolean on) {  
  Serial.println(F("Got relay switch request"));

  if (subcommand <= 0 || subcommand > RELAYS_NR)  {
    Serial.print(F("Unknown relay number: "));
    Serial.println(subcommand);
    return;
  }
  unsigned int relay_pin = relay_pins[subcommand - 1];

  if (on) {
    digitalWrite(relay_pin, HIGH);
  } else {
    digitalWrite(relay_pin, LOW);
  }

  CAN_Payload payload = init_relay_switch_response(subcommand, true);
  send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_SWITCH_RELAY_REPLY, 8, payload);
}

void handle_switch_oc(unsigned int subcommand, unsigned int src_node_id, boolean on) {  
  Serial.println(F("Got oc switch request"));

  if (subcommand <= 0 || subcommand > OC_PINS_NR)  {
    Serial.print(F("Unknown oc output number: "));
    Serial.println(subcommand);
    return;
  }
  unsigned int oc_pin = oc_pins[subcommand - 1];

  if (on) {
    digitalWrite(oc_pin, HIGH);
  } else {
    digitalWrite(oc_pin, LOW);
  }

  CAN_Payload payload = init_oc_switch_response(subcommand, true);
  send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_SWITCH_OC_REPLY, 8, payload);
}

void handle_relay_read(unsigned int subcommand, unsigned int src_node_id) {
  Serial.println(F("Got relay read rq"));

  if (subcommand > RELAYS_NR)  {
    Serial.print(F("Unknown relay subcommand read : "));
    Serial.println(subcommand);
    return;
  }

  CAN_Payload payload = init_relay_read_response(subcommand);
  send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_RELAY, 8, payload);
}

void handle_oc_read(unsigned int subcommand, unsigned int src_node_id) {
  Serial.println(F("Got oc read rq"));

  if (subcommand > OC_PINS_NR)  {
    Serial.print(F("Unknown oc subcommand read : "));
    Serial.println(subcommand);
    return;
  }

  CAN_Payload payload = init_oc_read_response(subcommand);
  send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_OC, 8, payload);
}

void handle_analog_read(unsigned int subcommand, unsigned int src_node_id) {
  Serial.println(F("Got analog read rq"));

  if (subcommand > NR_ANALOG_PINS)  {
    Serial.print(F("Unknown subcommand read : "));
    Serial.println(subcommand);
    return;
  }

  if (subcommand == 0) {
    /* read all */
    for (int i = 0; i < NR_ANALOG_PINS; i++) {
      CAN_Payload payload = init_analog_sensor_payload(analog_values[i]);
      set_aterminal_id(analog_terminals_ids[i], &payload);
      send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_ALL, 8, payload);
    }
    return;
  }

  CAN_Payload payload = init_analog_sensor_payload(analog_values[subcommand - 1]);
  set_aterminal_id(analog_terminals_ids[subcommand - 1], &payload);
  send_data_to_node(src_node_id, (CMD_EVENT << 8) | SC_EVENT_READ_ONE, 8, payload);
}

void handle_ping(unsigned int src_node_id, unsigned char data[8]) {
  Serial.println(F("Got ping rq"));
  CAN_Payload payload;
  memcpy(payload.data, data, 8);
  unsigned long pong = CMD_PONG << 8;
  send_data_to_node(src_node_id, pong, 8, payload);
}

CAN_Payload init_relay_switch_response(int relay_nr, bool success) {
  CAN_Payload payload = {0, 0, 0, 0, 0, 0, 0, success};
  payload.data[RELAY_PIN_BYTE] = relay_nr;
  return payload;
}

CAN_Payload init_relay_read_response(int relay_nr) {
  unsigned int relay_pin = relay_pins[relay_nr - 1];
  int state = digitalRead(relay_pin);
  CAN_Payload payload = {0, 0, 0, 0, 0, 0, 0, state};
  payload.data[RELAY_PIN_BYTE] = relay_nr;
  return payload;
}

CAN_Payload init_oc_switch_response(int oc_nr, bool success) {
  CAN_Payload payload = {0, 0, 0, 0, 0, 0, 0, success};
  payload.data[OC_PIN_BYTE] = oc_nr;
  return payload;
}

CAN_Payload init_oc_read_response(int oc_nr) {
  unsigned int oc_pin = oc_pins[oc_nr - 1];
  int state = digitalRead(oc_pin);
  CAN_Payload payload = {0, 0, 0, 0, 0, 0, 0, state};
  payload.data[OC_PIN_BYTE] = oc_nr;
  return payload;
}

CAN_Payload init_analog_sensor_payload(int value) {
  CAN_Payload payload = {0, 0, 0, 0, 0, 0, 0, 0};
  write_value_data(value, &payload);
  return payload;
}

void analog_send(int value, int id) {
  CAN_Payload payload = init_analog_sensor_payload(value);
  set_aterminal_id(id, &payload);
  send_data(8, payload);
}

void write_value_data(int value, CAN_Payload *payload) {
  payload->data[VAL_BYTEMS] = (value >> 8) & 0xFF;
  payload->data[VAL_BYTELS] = value & 0xFF;
}

void set_aterminal_id(int id, CAN_Payload *payload) {
  payload->data[AT_BYTE] = id;
}
