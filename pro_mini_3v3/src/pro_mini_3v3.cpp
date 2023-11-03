#include <Arduino.h>
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>

#include "joystick_parser.h"

// satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CSN_PIN_RADIO 2
#define CE_PIN_RADIO 3
#define SS_PIN_USB 10 // standard SS pin for USB Host Shield
#define CHAN_NUM 0x7A // radio channel (transmitter = receiver)
#define MODE_BUTTON 0x02

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

RF24 radio(CE_PIN_RADIO, CSN_PIN_RADIO); // radio module pins
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; // possible pipes numbers

int mode = 0;
byte transmit_data[5];
byte previous_data[5];

void usb_host_shield_init()
{
  #if !defined(__MIPSEL__)
  while (!Serial)
    ; // wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay(200);

  if (!Hid.SetReportParser(0, &Joy))
    ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
}

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
  Serial.begin(115200);

  pinMode(SS_PIN_USB, OUTPUT);
  usb_host_shield_init();
  transmitter_init();
}

void read_mode()
{
  if (Joy.data->buttons & MODE_BUTTON)
  {
    mode++;
    mode %= 2;
  }

  transmit_data[0] = mode;
}

void read_data()
{
  read_mode();

  int thrust_val = map(Joy.data->slider, 0, 255, 255, 0);
  transmit_data[1] = thrust_val;

  int roll_val = map(Joy.data->x, 0, 16383, 140, 40);
  transmit_data[2] = roll_val;

  int pitch_val = map(Joy.data->y, 0, 16383, 140, 40);
  transmit_data[3] = pitch_val;

  int yaw_val = map(Joy.data->twist, 0, 255, 140, 40);
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
  digitalWrite(SS_PIN_USB, LOW);
  Usb.Task();
  digitalWrite(SS_PIN_USB, HIGH);

  if (Joy.is_data_changed)
  {
    read_data();

    if (memcmp(transmit_data, previous_data, sizeof(transmit_data)) != 0)
    {
      print_data();
      send_data();
      memcpy(previous_data, transmit_data, sizeof(previous_data));
    }
  }

  Joy.is_data_changed = false;
}
