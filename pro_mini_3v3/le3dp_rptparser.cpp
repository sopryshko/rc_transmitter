#include "le3dp_rptparser.h"

JoystickReportParser::JoystickReportParser(JoystickEvents *evt) :
	joyEvents(evt)
{}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
	// Checking if there are changes in report since the method was last called
	if (memcmp(oldPad, buf, RPT_GAMEPAD_LEN) != 0 && joyEvents)
	{
		is_new_data = true;
		data = (GamePadEventData*)buf;
		// Calling Game Pad event handler
		//joyEvents->OnGamePadChanged((const GamePadEventData*)buf);
		memcpy(oldPad, buf, RPT_GAMEPAD_LEN);
	}
	else
	{
		is_new_data = false;
	}
	Serial.print(" Hello ");
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt)
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

