/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRLeftControllerInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRLeftControllerInputDevice::FAirVRLeftControllerInputDevice() 
{
    AddControlTransform((uint8)AirVRControllerKey::Transform);
    AddControlAxis2D((uint8)AirVRControllerKey::Axis2DThumbstick);
    AddControlAxis((uint8)AirVRControllerKey::Axis1DIndexTrigger);
    AddControlAxis((uint8)AirVRControllerKey::Axis1DHandTrigger);
    AddControlButton((uint8)AirVRControllerKey::ButtonThumbstick);
    AddControlButton((uint8)AirVRControllerKey::ButtonOne);
    AddControlButton((uint8)AirVRControllerKey::ButtonTwo);
    AddControlButton((uint8)AirVRControllerKey::ButtonSystem);

    AddExtControlButton((uint8)AirVRControllerKey::ButtonIndexTrigger);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonHandTrigger);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickUp);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickDown);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickLeft);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickRight);
}

void FAirVRLeftControllerInputDevice::UpdateExtendedControls() 
{
    const float AxisAsButtonThreshold = 0.5f;

    SetExtControlButton((uint8)AirVRControllerKey::ButtonIndexTrigger, GetAxis((uint8)AirVRControllerKey::Axis1DIndexTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonHandTrigger, GetAxis((uint8)AirVRControllerKey::Axis1DHandTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);

    ONAIRVR_VECTOR2D axis = GetAxis2D((uint8)AirVRControllerKey::Axis2DThumbstick);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickUp, axis.y >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickDown, axis.y <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickLeft, axis.x <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonThumbstickRight, axis.x >= AxisAsButtonThreshold ? 1.0f : 0.0f);
}
