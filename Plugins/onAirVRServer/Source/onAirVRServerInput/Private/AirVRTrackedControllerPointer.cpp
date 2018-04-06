/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRTrackedControllerPointer.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerInput.h"

float UAirVRTrackedControllerPointer::GetPrimaryButtonAnalogState(APlayerController* PlayerController) const
{
    return PlayerController->GetInputAnalogKeyState(EKeys::MotionController_Left_Thumbstick) + PlayerController->GetInputAnalogKeyState(EKeys::MotionController_Left_Trigger);
}

