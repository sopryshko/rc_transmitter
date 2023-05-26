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

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

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

void loop()
{
  Usb.Task();

  if (Joy.is_data_new)
  {
    Serial.print("X: ");
    Serial.print(Joy.data->x);
    Serial.print(" Y: ");
    Serial.print(Joy.data->y);
    // Serial.print(" Hat Switch: ");
    // Serial.print(Joy.data->hat);
    Serial.print(" Twist: ");
    Serial.print(Joy.data->twist);
    Serial.print(" Slider: ");
    Serial.print(Joy.data->slider);
    Serial.print(" Buttons: ");
    Serial.println(Joy.data->buttons, HEX);
  }

  Joy.is_data_new = false;
}
