/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerFunctionLibrary.h"
#include "AirVRServerPrivate.h"

#include "AirVRServerHMD.h"

static FName SystemName(TEXT("onAirVRServer"));
static FAirVRServerHMD* GetHMD()
{
    if (GEngine->XRSystem.IsValid() && GEngine->XRSystem.Get()->GetSystemName() == SystemName) {
        return static_cast<FAirVRServerHMD*>(GEngine->XRSystem.Get());
    }
    return nullptr;
}

UAirVRServerFunctionLibrary::UAirVRServerFunctionLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{}

bool UAirVRServerFunctionLibrary::IsPlayerBound(int32 PlayerControllerID)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->IsPlayerBound(PlayerControllerID);
    }
    return false;
}

bool UAirVRServerFunctionLibrary::IsPlayerActivated(int32 PlayerControllerID)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->IsPlayerActivated(PlayerControllerID);
    }
    return false;
}

bool UAirVRServerFunctionLibrary::GetConfig(int32 PlayerControllerID, class UAirVRClientConfig*& Config)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->GetConfig(PlayerControllerID, Config);
    }
    return false;
}

void UAirVRServerFunctionLibrary::GetOrientationAndPosition(int32 PlayerControllerID, FRotator& Rotation, FVector& Position)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        FQuat Orientation;
        HMD->GetOrientationAndPosition(PlayerControllerID, Orientation, Position);

        Rotation = Orientation.Rotator();
    }
    else {
        Rotation = FRotator::ZeroRotator;
        Position = FVector::ZeroVector;
    }
}

void UAirVRServerFunctionLibrary::ResetOrientationAndPosition(int32 PlayerControllerID)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->ResetOrientationAndPosition(PlayerControllerID);
    }
}

void UAirVRServerFunctionLibrary::AdjustBitrate(int32 PlayerControllerID, int32 BitrateInKbps)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->AdjustBitrate(PlayerControllerID, BitrateInKbps);
    }
}

void UAirVRServerFunctionLibrary::Disconnect(int32 PlayerControllerID)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->Disconnect(PlayerControllerID);
    }
}

bool UAirVRServerFunctionLibrary::IsInputDeviceAvailable(int32 PlayerControllerID, FAirVRInputDeviceType Device)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->IsInputDeviceAvailable(PlayerControllerID, Device);
    }
    return false;
}

void UAirVRServerFunctionLibrary::GetTrackedDeviceOrientationAndPosition(int32 PlayerControllerID, FAirVRInputDeviceType Device, FRotator& Rotation, FVector& Position)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->GetTrackedDeviceRotationAndPosition(PlayerControllerID, Device, Rotation, Position);
    }
}

void UAirVRServerFunctionLibrary::SetCameraRigTrackingModel(int32 PlayerControllerID, FAirVRTrackingModelType TrackingModelType)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->SetCameraRigTrackingModel(PlayerControllerID, TrackingModelType);
    }
}

void UAirVRServerFunctionLibrary::UpdateCameraRigExternalTrackerLocationAndRotation(int32 PlayerControllerID, FVector Location, FRotator Rotation)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->UpdateCameraRigExternalTrackerLocationAndRotation(PlayerControllerID, Location, Rotation.Quaternion());
    }
}

void UAirVRServerFunctionLibrary::SendAudioFrame(int32 PlayerControllerID, const float* Data, int SampleCount, int Channels, double TimeStamp)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->SendAudioFrame(PlayerControllerID, Data, SampleCount, Channels, TimeStamp);
    }
}

void UAirVRServerFunctionLibrary::SendAudioFrameToAllPlayers(const float* Data, int SampleCount, int Channels, double TimeStamp)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->SendAudioFrameToAllPlayers(Data, SampleCount, Channels, TimeStamp);
    }
}

void UAirVRServerFunctionLibrary::EnableNetworkTimeWarp(int32 PlayerControllerID, bool bEnable)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->EnableNetworkTimeWarp(PlayerControllerID, bEnable);
    }
}

bool UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(int32 PlayerControllerID, FAirVRInputDeviceType Device)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->IsDeviceFeedbackEnabled(PlayerControllerID, Device);
    }
    return false;
}

void UAirVRServerFunctionLibrary::EnableTrackedDeviceFeedback(int32 PlayerControllerID, FAirVRInputDeviceType Device, FString CookieTextureFile, float DepthScaleMultiplier)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->EnableTrackedDeviceFeedback(PlayerControllerID, Device, CookieTextureFile, DepthScaleMultiplier);
    }
}

void UAirVRServerFunctionLibrary::DisableDeviceFeedback(int32 PlayerControllerID, FAirVRInputDeviceType Device)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->DisableDeviceFeedback(PlayerControllerID, Device);
    }
}

void UAirVRServerFunctionLibrary::FeedbackTrackedDevice(int32 PlayerControllerID, FAirVRInputDeviceType Device, const FVector& RayOrigin, const FVector& HitPosition, const FVector& HitNormal)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->FeedbackTrackedDevice(PlayerControllerID, Device, RayOrigin, HitPosition, HitNormal);
    }
}

void UAirVRServerFunctionLibrary::GetCurrentPlayers(TArray<int32>& Result)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->GetCurrentPlayers(Result);
    }
}

void UAirVRServerFunctionLibrary::GetInputTransform(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 ControlID, FVector& Position, FQuat& Orientation)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->GetInputTransform(PlayerControllerID, Device, ControlID, Position, Orientation);
    }
}

FVector2D UAirVRServerFunctionLibrary::GetInputAxis2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->GetInputAxis2D(PlayerControllerID, Device, Control);
    }
    return FVector2D::ZeroVector;
}

float UAirVRServerFunctionLibrary::GetInputAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->GetInputAxis(PlayerControllerID, Device, Control);
    }
    return 0.0f;
}

float UAirVRServerFunctionLibrary::GetInputButtonRaw(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->GetInputButtonRaw(PlayerControllerID, Device, Control);
    }
    return 0.0f;
}

bool UAirVRServerFunctionLibrary::GetInputButton(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->GetInputButton(PlayerControllerID, Device, Control);
    }
    return false;
}

bool UAirVRServerFunctionLibrary::GetInputButtonDown(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->GetInputButtonDown(PlayerControllerID, Device, Control);
    }
    return false;
}

bool UAirVRServerFunctionLibrary::GetInputButtonUp(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        return HMD->GetInputButtonUp(PlayerControllerID, Device, Control);
    }
    return false;
}
