/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRTrackedControllerInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRTrackedControllerInputDevice::FAirVRTrackedControllerInputDevice()
{
    AddControlTouch((uint8)AirVRTrackedControllerKey::Touchpad);
    AddControlTransform((uint8)AirVRTrackedControllerKey::Transform);
    AddControlButton((uint8)AirVRTrackedControllerKey::ButtonTouchpad);
    AddControlButton((uint8)AirVRTrackedControllerKey::ButtonBack);
    AddControlButton((uint8)AirVRTrackedControllerKey::ButtonIndexTrigger);
    AddControlButton((uint8)AirVRTrackedControllerKey::ButtonUp);
    AddControlButton((uint8)AirVRTrackedControllerKey::ButtonDown);
    AddControlButton((uint8)AirVRTrackedControllerKey::ButtonLeft);
    AddControlButton((uint8)AirVRTrackedControllerKey::ButtonRight);

    AddExtControlAxis2D((uint8)AirVRTrackedControllerKey::ExtAxis2DTouchPosition);
    AddExtControlButton((uint8)AirVRTrackedControllerKey::ExtButtonTouch);
}

void FAirVRTrackedControllerInputDevice::UpdateExtendedControls()
{
    ONAIRVR_VECTOR2D Position;
    bool bTouch = false;

    GetTouch((uint8)AirVRTrackedControllerKey::Touchpad, &Position, &bTouch);
    SetExtControlAxis2D((uint8)AirVRTrackedControllerKey::ExtAxis2DTouchPosition, Position);
    SetExtControlButton((uint8)AirVRTrackedControllerKey::ExtButtonTouch, bTouch ? 1.0f : 0.0f);
}
