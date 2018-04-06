/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRGamepadInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRGamepadInputDevice::FAirVRGamepadInputDevice()
{
    AddControlAxis2D((uint8)AirVRGamepadKey::Axis2DLThumbstick);
    AddControlAxis2D((uint8)AirVRGamepadKey::Axis2DRThumbstick);
    AddControlAxis((uint8)AirVRGamepadKey::AxisLIndexTrigger);
    AddControlAxis((uint8)AirVRGamepadKey::AxisRIndexTrigger);

    AddControlButton((uint8)AirVRGamepadKey::ButtonA);
    AddControlButton((uint8)AirVRGamepadKey::ButtonB);
    AddControlButton((uint8)AirVRGamepadKey::ButtonX);
    AddControlButton((uint8)AirVRGamepadKey::ButtonY);
    AddControlButton((uint8)AirVRGamepadKey::ButtonStart);
    AddControlButton((uint8)AirVRGamepadKey::ButtonBack);
    AddControlButton((uint8)AirVRGamepadKey::ButtonLShoulder);
    AddControlButton((uint8)AirVRGamepadKey::ButtonRShoulder);
    AddControlButton((uint8)AirVRGamepadKey::ButtonLThumbstick);
    AddControlButton((uint8)AirVRGamepadKey::ButtonRThumbstick);
    AddControlButton((uint8)AirVRGamepadKey::ButtonDpadUp);
    AddControlButton((uint8)AirVRGamepadKey::ButtonDpadDown);
    AddControlButton((uint8)AirVRGamepadKey::ButtonDpadLeft);
    AddControlButton((uint8)AirVRGamepadKey::ButtonDpadRight);

    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonLIndexTrigger);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonRIndexTrigger);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickUp);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickDown);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickLeft);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickRight);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickUp);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickDown);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickLeft);
    AddExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickRight);
}

void FAirVRGamepadInputDevice::UpdateExtendedControls()
{
    const float AxisAsButtonThreshold = 0.5f;

    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonLIndexTrigger, GetAxis((uint8)AirVRGamepadKey::AxisLIndexTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonRIndexTrigger, GetAxis((uint8)AirVRGamepadKey::AxisRIndexTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    
    ONAIRVR_VECTOR2D Axis = GetAxis2D((uint8)AirVRGamepadKey::Axis2DLThumbstick);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickLeft, Axis.x <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickRight, Axis.x >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickDown, Axis.y <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonLThumbstickUp, Axis.y >= AxisAsButtonThreshold ? 1.0f : 0.0f);

    Axis = GetAxis2D((uint8)AirVRGamepadKey::Axis2DRThumbstick);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickLeft, Axis.x <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickRight, Axis.x >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickDown, Axis.y <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRGamepadKey::ExtButtonRThumbstickUp, Axis.y >= AxisAsButtonThreshold ? 1.0f : 0.0f);
}
