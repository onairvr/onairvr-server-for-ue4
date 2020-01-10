/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRControllerInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRControllerInputDevice::FAirVRControllerInputDevice() {
    AddControlTouch((uint8)AirVRControllerKey::Touchpad);
    
    AddControlButton((uint8)AirVRControllerKey::ButtonTouchpad);
    AddControlButton((uint8)AirVRControllerKey::ButtonUp);
    AddControlButton((uint8)AirVRControllerKey::ButtonDown);
    AddControlButton((uint8)AirVRControllerKey::ButtonLeft);
    AddControlButton((uint8)AirVRControllerKey::ButtonRight);

    AddControlAxis2D((uint8)AirVRControllerKey::Axis2DLThumbstick);
    AddControlAxis2D((uint8)AirVRControllerKey::Axis2DRThumbstick);
    AddControlAxis((uint8)AirVRControllerKey::AxisLIndexTrigger);
    AddControlAxis((uint8)AirVRControllerKey::AxisRIndexTrigger);
    AddControlAxis((uint8)AirVRControllerKey::AxisLHandTrigger);
    AddControlAxis((uint8)AirVRControllerKey::AxisRHandTrigger);
    AddControlButton((uint8)AirVRControllerKey::ButtonA);
    AddControlButton((uint8)AirVRControllerKey::ButtonB);
    AddControlButton((uint8)AirVRControllerKey::ButtonX);
    AddControlButton((uint8)AirVRControllerKey::ButtonY);
    AddControlButton((uint8)AirVRControllerKey::ButtonStart);
    AddControlButton((uint8)AirVRControllerKey::ButtonBack);
    AddControlButton((uint8)AirVRControllerKey::ButtonLThumbstick);
    AddControlButton((uint8)AirVRControllerKey::ButtonRThumbstick);
    AddControlButton((uint8)AirVRControllerKey::ButtonLShoulder);
    AddControlButton((uint8)AirVRControllerKey::ButtonRShoulder);

    AddExtControlAxis2D((uint8)AirVRControllerKey::Axis2DTouchpad);
    AddExtControlButton((uint8)AirVRControllerKey::TouchTouchpad);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonLIndexTrigger);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonRIndexTrigger);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonLHandTrigger);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonRHandTrigger);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickUp);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickDown);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickLeft);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickRight);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickUp);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickDown);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickLeft);
    AddExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickRight);
}

void FAirVRControllerInputDevice::UpdateExtendedControls() {
    const float AxisAsButtonThreshold = 0.5f;

    ONAIRVR_VECTOR2D axis2d;
    bool down = false;

    GetTouch((uint8)AirVRControllerKey::Touchpad, &axis2d, &down);
    SetExtControlAxis2D((uint8)AirVRControllerKey::Axis2DTouchpad, axis2d);
    SetExtControlButton((uint8)AirVRControllerKey::TouchTouchpad, down ? 1.0f : 0.0f);

    SetExtControlButton((uint8)AirVRControllerKey::ButtonLIndexTrigger, GetAxis((uint8)AirVRControllerKey::AxisLIndexTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonRIndexTrigger, GetAxis((uint8)AirVRControllerKey::AxisRIndexTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonLHandTrigger, GetAxis((uint8)AirVRControllerKey::AxisLHandTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonRHandTrigger, GetAxis((uint8)AirVRControllerKey::AxisRHandTrigger) >= AxisAsButtonThreshold ? 1.0f : 0.0f);

    axis2d = GetAxis2D((uint8)AirVRControllerKey::Axis2DLThumbstick);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickUp, axis2d.y >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickDown, axis2d.y <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickLeft, axis2d.x <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonLThumbstickRight, axis2d.x >= AxisAsButtonThreshold ? 1.0f : 0.0f);

    axis2d = GetAxis2D((uint8)AirVRControllerKey::Axis2DRThumbstick);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickUp, axis2d.y >= AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickDown, axis2d.y <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickLeft, axis2d.x <= -AxisAsButtonThreshold ? 1.0f : 0.0f);
    SetExtControlButton((uint8)AirVRControllerKey::ButtonRThumbstickRight, axis2d.x >= AxisAsButtonThreshold ? 1.0f : 0.0f);
}
