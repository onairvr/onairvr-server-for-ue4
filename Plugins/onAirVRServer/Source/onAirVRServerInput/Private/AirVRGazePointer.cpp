/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRGazePointer.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerInput.h"

float UAirVRGazePointer::GetPrimaryButtonAnalogState(APlayerController* PlayerController) const
{
    return PlayerController->GetInputAnalogKeyState(FAirVRInputKey::TouchpadTouch) + PlayerController->GetInputAnalogKeyState(EKeys::Gamepad_FaceButton_Bottom);
}
