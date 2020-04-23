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
private:
    struct FKeyNames {
        FName Main;
        FName Emulated;

        FKeyNames(FName InMain) : Main(InMain), Emulated(FName()) {}
        FKeyNames(FName InMain, FName InEmulated) : Main(InMain), Emulated(InEmulated) {}
    };

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
    void DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& KeyForX, const FKeyNames& KeyForY);
    void DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, 
                               const FKeyNames& LeftKeyForX, const FKeyNames& LeftKeyForY, const FKeyNames& RightKeyForX, const FKeyNames& RightKeyForY);
    void DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Key);
    void DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& LeftKey, const FKeyNames& RightKey);
    void DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Key);
    void DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& LeftKey, const FKeyNames& RightKey);
    void OnControllerAnalog(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Key, int32 ControllerID, float Value);
    void OnControllerButtonPressed(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Key, int32 ControllerID, bool IsRepeat);
    void OnControllerButtonReleased(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Key, int32 ControllerID, bool IsRepeat);

private:
    int32 ControllerID;
    FVector LeftControllerPosition;
    FQuat LeftControllerOrientation;
    FVector RightControllerPosition;
    FQuat RightControllerOrientation;
};
