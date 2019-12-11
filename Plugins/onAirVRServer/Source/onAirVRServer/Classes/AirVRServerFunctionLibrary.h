/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AirVRServerFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class FAirVRInputDeviceType : uint8 
{
    HeadTracker,
    LeftHandTracker,
    RightHandTracker,
    Controller
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
    static void AdjustBitrate(int32 PlayerControllerID, int32 BitrateInKbps);

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

    // for tracked device feedback
    static bool IsDeviceFeedbackEnabled(int32 PlayerControllerID, FAirVRInputDeviceType Device);
    static void EnableTrackedDeviceFeedback(int32 PlayerControllerID, FAirVRInputDeviceType Device, FString CookieTextureFile, float DepthScaleMultiplier);
    static void DisableDeviceFeedback(int32 PlayerControllerID, FAirVRInputDeviceType Device);
    static void FeedbackTrackedDevice(int32 PlayerControllerID, FAirVRInputDeviceType Device, const FVector& RayOrigin, const FVector& HitPosition, const FVector& HitNormal);

    // internal interfaces for onAirVRServerInput
    static void GetCurrentPlayers(TArray<int32>& Result);
    static void GetInputTransform(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector& Position, FQuat& Orientation);
    static FVector2D GetInputAxis2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static float GetInputAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static float GetInputButtonRaw(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static bool GetInputButton(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static bool GetInputButtonDown(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
    static bool GetInputButtonUp(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control);
};
