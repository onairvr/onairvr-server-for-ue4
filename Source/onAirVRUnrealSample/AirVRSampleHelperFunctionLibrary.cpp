/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "onAirVRUnrealSample.h"
#include "AirVRSampleHelperFunctionLibrary.h"

#include "AirVRServerInput.h"

static FVector2D TranslateTouchPositionToMoveDirection(FVector2D Pos)
{
    if (Pos.Size() > 0.3f) {
        if (Pos.GetAbs().X > Pos.GetAbs().Y) {
            return Pos.X > 0.0f ? FVector2D(1.0f, 0.0f) : FVector2D(-1.0f, 0.0f);
        }
        else {
            return Pos.Y > 0.0f ? FVector2D(0.0f, 1.0f) : FVector2D(0.0f, -1.0f);
        }
    }
    return FVector2D(0.0f, 0.0f);
}

UAirVRSampleHelperFunctionLibrary::UAirVRSampleHelperFunctionLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{}

void UAirVRSampleHelperFunctionLibrary::GetPlayerMoveInputDirection(APlayerController* PlayerController, FVector2D& OutResult)
{
    OutResult = FVector2D(0.0f, 0.0f);
    if (PlayerController->GetInputAnalogKeyState(FAirVRInputKey::TouchpadTouch) > 0.0f) {
        OutResult += TranslateTouchPositionToMoveDirection(FVector2D(PlayerController->GetInputAnalogKeyState(FAirVRInputKey::TouchpadPositionX) * 5.0f,
                                                                     PlayerController->GetInputAnalogKeyState(FAirVRInputKey::TouchpadPositionY) * 5.0f));
    }
    
    OutResult += TranslateTouchPositionToMoveDirection(FVector2D(PlayerController->GetInputAnalogKeyState(EKeys::MotionController_Right_Thumbstick_X),
                                                                    PlayerController->GetInputAnalogKeyState(EKeys::MotionController_Right_Thumbstick_Y)));
    OutResult += FVector2D(PlayerController->GetInputAnalogKeyState(EKeys::Gamepad_LeftX), PlayerController->GetInputAnalogKeyState(EKeys::Gamepad_LeftY));
}
