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

#define MODE_BUTTON 0x02

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

int mode = 0;
byte transmit_data[5];
byte previous_data[5];

void setup()
{
  Serial.begin(115200);

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
  int thrust_val = map(Joy.data->slider, 0, 255, 255, 0);
  transmit_data[1] = thrust_val;

  int roll_val = map(Joy.data->x, 0, 16383, 140, 40);
  transmit_data[2] = roll_val;

  int pitch_val = map(Joy.data->y, 0, 16383, 35, 145);
  transmit_data[3] = pitch_val;

  int yaw_val = map(Joy.data->twist, 0, 255, 150, 30);
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
  // radio.powerUp();
  // radio.write(&transmit_data, sizeof(transmit_data)); // send packet
  // radio.powerDown();
}

void loop()
{
  Usb.Task();

  if (Joy.is_data_changed)
  {
    read_mode();
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
