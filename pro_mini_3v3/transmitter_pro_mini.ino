#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>

#include "le3dp_rptparser.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
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
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
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
    if (Joy.is_new_data)
    {
      Serial.print("X: ");
      Serial.println(Joy.data->x);
//      Serial.print(" Y: ");
//      Serial.print(evt->y);
//      Serial.print(" Hat Switch: ");
//      Serial.print(evt->hat);
//      Serial.print(" Twist: ");
//      Serial.print(evt->twist);
//      Serial.print(" Slider: ");
//      Serial.print(evt->slider);
//      Serial.print(" Buttons: ");
//      Serial.println(evt->buttons, HEX);
    }

    Joy.is_new_data = false;
}
