/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRServerFunctionLibrary.h"
#include "InputCoreTypes.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

class FAirVRCameraRigInput
{
public:
    FAirVRCameraRigInput(int32 InControllerID) : 
        ControllerID(InControllerID), 
        LeftControllerPosition(FVector::ZeroVector), LeftControllerOrientation(FQuat::Identity),
        RightControllerPosition(FVector::ZeroVector), RightControllerOrientation(FQuat::Identity) {}
    ~FAirVRCameraRigInput() {}

public:
    bool IsControllerAvailable(const EControllerHand DeviceHand) const;

    void GetControllerOrientationAndPosition(const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition);
    void UpdateAndDispatch(FGenericApplicationMessageHandler& MessageHandler);

private:
    void DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName KeyForX, FName KeyForY);
    void DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKeyForX, FName LeftKeyForY, FName RightKeyForX, FName RightKeyForY);
    void DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName Key);
    void DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKey, FName RightKey);
    void DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName Key);
    void DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKey, FName RightKey);

private:
    int32 ControllerID;
    FVector LeftControllerPosition;
    FQuat LeftControllerOrientation;
    FVector RightControllerPosition;
    FQuat RightControllerOrientation;
};
