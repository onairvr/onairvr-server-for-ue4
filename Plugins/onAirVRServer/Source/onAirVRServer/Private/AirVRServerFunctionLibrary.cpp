/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

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

void UAirVRServerFunctionLibrary::UpdateRaycastHitResult(int32 PlayerControllerID, FAirVRInputDeviceType Device, const FVector& RayOrigin, const FVector& HitPosition, const FVector& HitNormal)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->UpdateRaycastHitResult(PlayerControllerID, Device, RayOrigin, HitPosition, HitNormal);
    }
}

void UAirVRServerFunctionLibrary::UpdateRenderOnClient(int32 PlayerControllerID, FAirVRInputDeviceType Device, bool bRenderOnClient) 
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->UpdateRenderOnClient(PlayerControllerID, Device, bRenderOnClient);
    }
}

void UAirVRServerFunctionLibrary::GetCurrentPlayers(TArray<int32>& Result)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->GetCurrentPlayers(Result);
    }
}

uint8 UAirVRServerFunctionLibrary::GetInputState(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputState(PlayerControllerID, Device, Control) : 0;
}

uint8 UAirVRServerFunctionLibrary::GetInputByteAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputByteAxis(PlayerControllerID, Device, Control) : 0;
}

float UAirVRServerFunctionLibrary::GetInputAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputAxis(PlayerControllerID, Device, Control) : 0;
}

FVector2D UAirVRServerFunctionLibrary::GetInputAxis2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputAxis2D(PlayerControllerID, Device, Control) : FVector2D::ZeroVector;
}

void UAirVRServerFunctionLibrary::GetInputPose(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector& Position, FQuat& Orientation)
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->GetInputPose(PlayerControllerID, Device, Control, Position, Orientation);
    }
}

void UAirVRServerFunctionLibrary::GetInputTouch2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector2D& Position, uint8& State) 
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->GetInputTouch2D(PlayerControllerID, Device, Control, Position, State);
    }
}

bool UAirVRServerFunctionLibrary::IsInputActive(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->IsInputActive(PlayerControllerID, Device, Control) : false;
}

bool UAirVRServerFunctionLibrary::IsInputActive(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->IsInputActive(PlayerControllerID, Device, Control, Direction) : false;
}

bool UAirVRServerFunctionLibrary::GetInputActivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputActivated(PlayerControllerID, Device, Control) : false;
}

bool UAirVRServerFunctionLibrary::GetInputActivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputActivated(PlayerControllerID, Device, Control, Direction) : false;
}

bool UAirVRServerFunctionLibrary::GetInputDeactivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputDeactivated(PlayerControllerID, Device, Control) : false;
}

bool UAirVRServerFunctionLibrary::GetInputDeactivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction) 
{
    FAirVRServerHMD* HMD = GetHMD();
    return HMD ? HMD->GetInputDeactivated(PlayerControllerID, Device, Control, Direction) : false;
}

void UAirVRServerFunctionLibrary::PendInputVibration(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, float Frequency, float Amplitude) 
{
    FAirVRServerHMD* HMD = GetHMD();
    if (HMD) {
        HMD->PendInputVibration(PlayerControllerID, Device, Control, Frequency, Amplitude);
    }
}
