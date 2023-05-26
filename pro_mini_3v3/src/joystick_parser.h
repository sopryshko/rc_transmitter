#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include <usbhid.h>

struct JoystickEventData
{
  uint16_t buttons;
  uint8_t hat;
  uint16_t x;
  uint16_t y;
  uint8_t twist;
  uint8_t slider;
};

#define RPT_JOYSTICK_LEN sizeof(JoystickEventData) / sizeof(uint8_t)

class JoystickEvents
{
public:
  virtual void OnGamePadChanged(const JoystickEventData *evt);
};

class JoystickReportParser : public HIDReportParser
{
  JoystickEvents *joyEvents;
  uint8_t oldPad[RPT_JOYSTICK_LEN];

public:
  bool is_data_new;
  JoystickEventData *data;

  JoystickReportParser(JoystickEvents *evt);
  virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

#endif // __HIDJOYSTICKRPTPARSER_H__
