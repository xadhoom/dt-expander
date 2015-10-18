#include "commands.h"

void send_data(int len, CAN_Payload payload) {
  send_data_to_node(CAN_MASTER, (CMD_EVENT << 8) | SC_EVENT_USC, len, payload);
}

void send_data_to_node(unsigned long dst_node_id, unsigned long cmd_subcmd, int len, CAN_Payload payload) {
  int can_res, i;
  static int frame_type = 1;

  unsigned long msg_id = (my_can_id << 23) | (dst_node_id << 16) | cmd_subcmd;

  for (i = 0; i < CAN_MAX_RETRY; i++) {
    can_res = CAN.sendMsgBuf(msg_id, frame_type, len, payload.data);

    if (can_res == CAN_OK)  {
      return;
    } else if (can_res == CAN_GETTXBFTIMEOUT) {
      Serial.print(F("Get buffer timeout, retry: "));
      Serial.println(i);
    } else if (can_res == CAN_SENDMSGTIMEOUT) {
      Serial.print(F("Message timeout, retry: "));
      Serial.println(i);
    } else {
      Serial.print(F("Generic error sending, WTF??? retry: "));
      Serial.println(i);
    }
  }
  Serial.print(F("Cannot send data after "));
  Serial.print(i);
  Serial.println(F(" retries. WTF???"));
  Serial.print(F("Can id :"));
  Serial.println(my_can_id);
  //Serial.print("Data :");
  //Serial.println(payload.byte);
}
