/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "onairvr_input.h"
#include "ocs_server.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "AirVRServerFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class FAirVRInputDeviceType : uint8 
{
    HeadTracker         = (uint8)AirVRInputDeviceID::HeadTracker,
    LeftHandTracker     = (uint8)AirVRInputDeviceID::LeftHandTracker,
    RightHandTracker    = (uint8)AirVRInputDeviceID::RightHandTracker,
    Controller          = (uint8)AirVRInputDeviceID::Controller,
    TouchScreen         = (uint8)AirVRInputDeviceID::TouchScreen
};

UENUM(BlueprintType)
enum class FAirVRTrackingModelType : uint8 
{
    Head,
    InterpupilaryDistanceOnly,
    ExternalTracker
};

UCLASS()
class ONAIRVRSERVER_API UAirVRServerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "onAirVR")
    static bool IsPlayerBound(int32 PlayerControllerID);

    UFUNCTION(BlueprintPure, Category = "onAirVR")
    static bool IsPlayerActivated(int32 PlayerControllerID);

    UFUNCTION(BlueprintCallable, Category = "onAirVR")
    static bool GetConfig(int32 PlayerControllerID, class UAirVRClientConfig*& Config);

    UFUNCTION(BlueprintPure, Category = "onAirVR")
    static void GetOrientationAndPosition(int32 PlayerControllerID, FRotator& Rotation, FVector& Position);

    UFUNCTION(BlueprintCallable, Category = "onAirVR")
    static void ResetOrientationAndPosition(int32 PlayerControllerID);

    UFUNCTION(BlueprintCallable, Category = "onAirVR")
    static void Disconnect(int32 PlayerControllerID);

    UFUNCTION(BlueprintCallable, Category = "onAirVR")
    static bool IsInputDeviceAvailable(int32 PlayerControllerID, FAirVRInputDeviceType Device);

    UFUNCTION(BlueprintPure, Category = "onAirVR")
    static void GetTrackedDeviceOrientationAndPosition(int32 PlayerControllerID, FAirVRInputDeviceType Device, FRotator& Rotation, FVector& Position);

    UFUNCTION(BlueprintCallable, Category = "onAirVR")
    static void SetCameraRigTrackingModel(int32 PlayerControllerID, FAirVRTrackingModelType TrackingModelType);

    UFUNCTION(BlueprintCallable, Category = "onAirVR")
    static void UpdateCameraRigExternalTrackerLocationAndRotation(int32 PlayerControllerID, FVector Location, FRotator Rotation);

    static void SendAudioFrame(int32 PlayerControllerID, const float* Data, int SampleCount, int Channels, double TimeStamp);
    static void SendAudioFrameToAllPlayers(const float* Data, int SampleCount, int Channels, double TimeStamp);

    static void EnableNetworkTimeWarp(int32 PlayerControllerID, bool bEnable);

    // for motion controller feedback
    static void UpdateRaycastHitResult(int32 PlayerControllerID, FAirVRInputDeviceType Device, const FVector& RayOrigin, const FVector& HitPosition, const FVector& HitNormal);
    static void UpdateRenderOnClient(int32 PlayerControllerID, FAirVRInputDeviceType Device, bool bRenderOnClient);

    // internal interfaces for onAirVRServerInput
    static void GetCurrentPlayers(TArray<int32>& Result);
    static uint8 GetInputState(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static uint8 GetInputByteAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static float GetInputAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static FVector2D GetInputAxis2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static void GetInputPose(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector& Position, FQuat& Orientation);
    static void GetInputTouch2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector2D& Position, uint8& State);
    static bool IsInputActive(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static bool IsInputActive(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction);
    static bool GetInputActivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static bool GetInputActivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction);
    static bool GetInputDeactivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static bool GetInputDeactivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction);
    static void PendInputVibration(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, float Frequency, float Amplitude);
};
