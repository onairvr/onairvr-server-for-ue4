/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRServerFunctionLibrary.h"
#include "InputCoreTypes.h"
#include "IHapticDevice.h"
#include "GenericPlatform/IInputInterface.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

class FAirVRCameraRigInput
{
private:
    typedef TArray<FName> FKeyNames;

public:
    FAirVRCameraRigInput(int32 InControllerID);
    ~FAirVRCameraRigInput() {}

public:
    bool IsControllerAvailable(EControllerHand DeviceHand) const;

    void GetControllerOrientationAndPosition(EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition);
    void SetHapticFeedback(EControllerHand DeviceHand, const FHapticFeedbackValues& Values);
    void SetForceFeedback(const FForceFeedbackValues& Values);
    void UpdateAndDispatch(FGenericApplicationMessageHandler& MessageHandler);

private:
    bool ParseDeviceType(EControllerHand DeviceHand, FAirVRInputDeviceType& Result) const;
    bool ParseDeviceType(FForceFeedbackChannelType Channel, FAirVRInputDeviceType& Result) const;
    int LastVibrationIndex(FAirVRInputDeviceType Device) const;
    void SetVibration(FAirVRInputDeviceType Device, float Frequency, float Amplitude, bool bIsHapticEffect);

    void DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8 Control, const FKeyNames& KeysForX, const FKeyNames& KeysForY);
    void DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Keys);
    void DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Keys);
    void DispatchControlDirectionButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Keys, OCS_INPUT_DIRECTION Direction);
    void OnControllerAnalog(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Keys, int32 ControllerID, float Value);
    void OnControllerButtonPressed(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Keys, int32 ControllerID, bool IsRepeat);
    void OnControllerButtonReleased(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Keys, int32 ControllerID, bool IsRepeat);

    void DispatchScreenTouches(FGenericApplicationMessageHandler& MessageHandler);

private:
    int32 ControllerID;
    FVector LeftControllerPosition;
    FQuat LeftControllerOrientation;
    FVector RightControllerPosition;
    FQuat RightControllerOrientation;
    bool bPlayingHapticEffect;
    FHapticFeedbackValues LastVibrationValue[2];
};
