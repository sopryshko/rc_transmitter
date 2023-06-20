#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CHAN_NUM 0x7A // radio channel (transmitter = receiver)
#define BUTTON 2
#define THRUST_IN A0
#define ROLL_IN A1
#define PITCH_IN A2
#define YAW_IN A3

RF24 radio(9, 10); // radio module pins
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; // possible pipes numbers

byte transmit_data[5];
byte previous_data[5];

int mode = 0;
int cur_button_state = HIGH;
int prev_button_state = HIGH;
unsigned long prev_debounce_time = 0;
unsigned long debounce_delay = 20;

void transmitter_init()
{
  radio.begin(); // radio module activation
  radio.setAutoAck(true); // respond to the received packet (transmitter = receiver)
  radio.setRetries(0, 15); // number of retry attempts and delay
  radio.enableAckPayload();
  radio.setPayloadSize(32); // packet size (bytes)

  radio.openWritingPipe(address[0]); // transmitter
  radio.setChannel(CHAN_NUM);

  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX (transmitter = receiver)
  // the lower the data rate, the higher the range
  radio.setDataRate(RF24_250KBPS); // RF24_2MBPS, RF24_1MBPS, RF24_250KBPS (transmitter = receiver)

  radio.powerUp();
  radio.stopListening(); // transmitter
}

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);

  transmitter_init();
}

bool is_debounced(int input)
{
  int reading = digitalRead(input);
  if (reading != prev_button_state)
    prev_debounce_time = millis(); // reset the debouncing timer

  prev_button_state = reading;

  if ((millis() - prev_debounce_time) > debounce_delay)
  {
    if (reading != cur_button_state)
    {
      cur_button_state = reading;
      if (cur_button_state == HIGH)
        return true;
    }
  }

  return false;
}

void read_mode()
{
  if (is_debounced(BUTTON))
  {
    mode++;
    mode %= 2;
  }

  transmit_data[0] = mode;
}

void read_controllers()
{
  read_mode();
  
  int thrust_val = map(analogRead(THRUST_IN), 0, 1023, 0, 255);
  transmit_data[1] = thrust_val;

  int roll_val = analogRead(ROLL_IN);
  roll_val = map(roll_val, 0, 1023, 40, 140);
  transmit_data[2] = roll_val;

  int pitch_val = analogRead(PITCH_IN);
  pitch_val = map(pitch_val, 0, 1023, 35, 145);
  transmit_data[3] = pitch_val;

  int yaw_val = analogRead(YAW_IN);
  yaw_val = map(yaw_val, 0, 1023, 30, 150);
  transmit_data[4] = yaw_val;
}

void print_data()
{
  size_t data_size = sizeof(transmit_data) / sizeof(transmit_data[0]);
  // mode thrust roll pitch yaw
  for (size_t i = 0; i < data_size; i++)
  {
    Serial.print(transmit_data[i]);
    Serial.print(' ');
  }

  Serial.println();
}

void send_data()
{
  radio.powerUp();
  radio.write(&transmit_data, sizeof(transmit_data)); // send packet
  radio.powerDown();
}

void loop()
{
  read_controllers();
  // checking if there are changes in data
  if (memcmp(transmit_data, previous_data, sizeof(transmit_data)) != 0)
  {
    print_data();
    send_data();
    memcpy(previous_data, transmit_data, sizeof(previous_data));
  }
}
