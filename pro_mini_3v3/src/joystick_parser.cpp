#include "joystick_parser.h"

JoystickReportParser::JoystickReportParser(JoystickEvents *evt) : joyEvents(evt)
{
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  // checking if there are changes in report since the method was last called
  if (memcmp(oldPad, buf, RPT_JOYSTICK_LEN) != 0 && joyEvents)
  {
    is_data_changed = true;
    data = (JoystickEventData *)buf;
    // calling event handler
    // joyEvents->OnJoystickChanged((const JoystickEventData*)buf);
    memcpy(oldPad, buf, RPT_JOYSTICK_LEN);
  }
  else
    is_data_changed = false;
}

void JoystickEvents::OnJoystickChanged(const JoystickEventData *evt)
{
  Serial.print("X: ");
  Serial.print(evt->x);
  Serial.print(" Y: ");
  Serial.print(evt->y);
  Serial.print(" Hat Switch: ");
  Serial.print(evt->hat);
  Serial.print(" Twist: ");
  Serial.print(evt->twist);
  Serial.print(" Slider: ");
  Serial.print(evt->slider);
  Serial.print(" Buttons: ");
  Serial.println(evt->buttons, HEX);
}
