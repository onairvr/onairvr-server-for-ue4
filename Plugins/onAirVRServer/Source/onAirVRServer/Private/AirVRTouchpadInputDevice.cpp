/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRTouchpadInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRTouchpadInputDevice::FAirVRTouchpadInputDevice()
{
    AddControlTouch((uint8)AirVRTouchpadKey::Touchpad); 
    AddControlButton((uint8)AirVRTouchpadKey::ButtonBack);
    AddControlButton((uint8)AirVRTouchpadKey::ButtonUp);
    AddControlButton((uint8)AirVRTouchpadKey::ButtonDown);
    AddControlButton((uint8)AirVRTouchpadKey::ButtonLeft);
    AddControlButton((uint8)AirVRTouchpadKey::ButtonRight);

    AddExtControlAxis2D((uint8)AirVRTouchpadKey::ExtAxis2DPosition);
    AddExtControlButton((uint8)AirVRTouchpadKey::ExtButtonTouch);
}

void FAirVRTouchpadInputDevice::UpdateExtendedControls()
{
    ONAIRVR_VECTOR2D Position;
    bool bTouch = false;

    GetTouch((uint8)AirVRTouchpadKey::Touchpad, &Position, &bTouch);

    SetExtControlAxis2D((uint8)AirVRTouchpadKey::ExtAxis2DPosition, Position);
    SetExtControlButton((uint8)AirVRTouchpadKey::ExtButtonTouch, bTouch ? 1.0f : 0.0f);
}
