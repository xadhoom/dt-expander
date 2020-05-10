#include "detectino.h"

#include <Filters.h>
#include <FilterOnePole.h>

#include <Timer.h>

#include <mcp_can.h>
#include <SPI.h>

/* current values */
unsigned int analog_values[NR_ANALOG_PINS];
unsigned int analog_old_values[NR_ANALOG_PINS];

/*
 * ALIVE led
 */
int ALIVE = LOW;

/*
 * LOWPASS Filters Stuff
 */
float FilterFrequency = 5;
FilterOnePole analog_filters[NR_ANALOG_PINS];

/*
 * CAN INIT STUFF
 */
unsigned long my_can_id;
MCP_CAN CAN(CAN_CS_PIN);

/*
 * Timers
 */
Timer t_inputs;
Timer t_alive;
Timer t_check;
Timer t_notify;

void setup()
{
  Serial.begin(115200);

START_INIT:

  if (CAN_OK == CAN.begin(CAN_500KBPS))                  // init can bus : baudrate = 500k
  {
    Serial.println(F("CAN BUS Shield init ok!"));
  }
  else
  {
    Serial.println(F("CAN BUS Shield init fail"));
    Serial.println(F("Init CAN BUS Shield again"));
    delay(1000);
    goto START_INIT;
  }

START_NODE_ID:
  setup_id();
  my_can_id = read_id();
  if (my_can_id == 0x0 || my_can_id >= CAN_BCAST) {
    Serial.println(F("ID cannot be 0x0 or bigger than 0x7f, restarting!"));
    delay(1000);
    goto START_NODE_ID;
  }

  setup_alive();
  setup_inputs();
  setup_outputs();
  setup_analogReads();
  setup_checkAlert();
  setup_notify();

  /* Setup Masks */
  CAN.init_Mask(0, 1, CAN_DST_MASK);
  CAN.init_Mask(1, 1, CAN_DST_MASK);
  CAN.init_Filt(0, 1, CAN_BCAST_FILT);
  CAN.init_Filt(1, 1, my_can_id << 16); /* shift right since we need to filter as dst, see detectino.h */
}

void setup_alive() {
  pinMode(ALIVE_PIN, OUTPUT);
  t_alive.every(250, aliveLed);
}

void setup_analogReads() {
  for (int i = 0; i < NR_ANALOG_PINS; i++) {
    analog_filters[i] = FilterOnePole(LOWPASS, FilterFrequency);
  }
  t_inputs.every(1, feed_sensors);
}

void setup_checkAlert() {
  t_check.every(100, check_alert);
}

void setup_notify() {
  t_notify.every(1000, notify);
}

void setup_id() {
  pinMode(ID_PIN1, INPUT_PULLUP);
  pinMode(ID_PIN2, INPUT_PULLUP);
  pinMode(ID_PIN3, INPUT_PULLUP);
  pinMode(ID_PIN4, INPUT_PULLUP);
}

void setup_inputs() {
  for (int i = 0; i < NR_ANALOG_PINS; i++) {
    analog_values[i] = 0;
    analog_old_values[i] = 0;
  }
}

void setup_outputs() {
  for (int i = 0; i < RELAYS_NR; i++) {
    pinMode(relay_pins[i], OUTPUT);
    digitalWrite(relay_pins[i], LOW);
  }
  for (int i = 0; i < OC_PINS_NR; i++) {
    pinMode(oc_pins[i], OUTPUT);
    digitalWrite(oc_pins[i], LOW);
  }
}

void aliveLed() {
  ALIVE = !ALIVE;
  digitalWrite(ALIVE_PIN, ALIVE);
}

unsigned int read_id() {
  int id;
  id = !digitalRead(ID_PIN4) << 3 | !digitalRead(ID_PIN3) << 2 | !digitalRead(ID_PIN2) << 1 | !digitalRead(ID_PIN1);
  return id;
}

void feed_sensors() {
  // feed analog data to filter
  for (int i = 0; i < NR_ANALOG_PINS; i++) {
    analog_filters[i].input(analogRead(analog_terminals[i]));
  }
}

void read_sensors() {
  for (int i = 0; i < NR_ANALOG_PINS; i++) {
    analog_old_values[i] = analog_values[i];
    //analog_values[i] = analog_filters[i].output();
    analog_values[i] = analogRead(analog_terminals[i]);
  }
}

int acmp(unsigned int a, unsigned int b) {
  /* if the readings fluctuate between +/-10 (~50mV) ignore them */
  long low = (long)b - 10;
  long high = (long)b + 10;
  long tmp = (long)a;

  if (tmp >= low &&  tmp <= high) {
    return 0;
  }
  return 1;
}

void check_alert() {
  read_sensors();

  for (int i = 0; i < NR_ANALOG_PINS; i++) {
    if ( acmp(analog_values[i], analog_old_values[i]) ) {
      analog_send(analog_values[i], analog_terminals_ids[i]);
      Serial.print("Alert! sending value ");
      Serial.print(analog_values[i], DEC);
      Serial.print(" from port ");
      Serial.println(i, DEC);
    }
  }
}

void notify() {
  for (int i = 0 ; i < NR_ANALOG_PINS; i++) {
    analog_send(analog_values[i], analog_terminals_ids[i]);
  }
}

void loop()
{
  t_inputs.update();
  t_alive.update();
  t_check.update();
  t_notify.update();
  read_from_bus();
}
