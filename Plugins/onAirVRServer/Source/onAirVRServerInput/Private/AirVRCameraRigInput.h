/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRServerFunctionLibrary.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

class FAirVRCameraRigInput
{
public:
    FAirVRCameraRigInput(int32 InControllerID) : ControllerID(InControllerID), ControllerPosition(FVector::ZeroVector), ControllerOrientation(FQuat::Identity) {}
    ~FAirVRCameraRigInput() {}

public:
    void UpdateAndDispatch(FGenericApplicationMessageHandler& MessageHandler);
    void GetControllerOrientationAndPosition(FRotator& OutOrientation, FVector& OutPosition);

private:
    void DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName KeyForX, FName KeyForY);
    void DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKeyForX, FName LeftKeyForY, FName RightKeyForX, FName RightKeyForY);
    void DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName Key);
    void DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKey, FName RightKey);
    void DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName Key);
    void DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKey, FName RightKey);

private:
    int32 ControllerID;
    FVector ControllerPosition;
    FQuat ControllerOrientation;
};
