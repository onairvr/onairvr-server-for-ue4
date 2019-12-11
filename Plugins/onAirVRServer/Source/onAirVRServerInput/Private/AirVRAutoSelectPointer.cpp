/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRAutoSelectPointer.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerInput.h"

UAirVRAutoSelectPointer::UAirVRAutoSelectPointer()
{
    CurrentDevice = FAirVRInputDeviceType::HeadTracker;
}

void UAirVRAutoSelectPointer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    FAirVRInputDeviceType Device = UAirVRServerFunctionLibrary::IsInputDeviceAvailable(PlayerControllerID, FAirVRInputDeviceType::RightHandTracker) ? FAirVRInputDeviceType::RightHandTracker : FAirVRInputDeviceType::HeadTracker;
    if (Device != CurrentDevice) {
        if (UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(PlayerControllerID, CurrentDevice)) {
            UAirVRServerFunctionLibrary::DisableDeviceFeedback(PlayerControllerID, CurrentDevice);
        }
        CurrentDevice = Device;
    }

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UAirVRAutoSelectPointer::GetPrimaryButtonAnalogState(APlayerController* PlayerController) const
{
    if (CurrentDevice == FAirVRInputDeviceType::HeadTracker) {
        return PlayerController->GetInputAnalogKeyState(FAirVRInputKey::TouchpadTouch) + PlayerController->GetInputAnalogKeyState(EKeys::Gamepad_FaceButton_Bottom);
    }
    else if (CurrentDevice == FAirVRInputDeviceType::RightHandTracker) {
        return PlayerController->GetInputAnalogKeyState(EKeys::MotionController_Left_Thumbstick) + PlayerController->GetInputAnalogKeyState(EKeys::MotionController_Left_Trigger);
    }
    return 0.0f;
}
