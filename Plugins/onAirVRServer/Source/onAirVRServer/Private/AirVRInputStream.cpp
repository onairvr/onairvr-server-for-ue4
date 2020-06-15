/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRInputStream.h"
#include "AirVRServerPrivate.h"

#include "AirVRCameraRig.h"
#include "AirVRHeadTrackerInputDevice.h"
#include "AirVRLeftHandTrackerInputDevice.h"
#include "AirVRRightHandTrackerInputDevice.h"
#include "AirVRControllerInputDevice.h"
#include "AirVRInputDeviceFeedback.h"

FAirVRInputStream::FAirVRInputStream(FAirVRCameraRig* InOwner)
    : Owner(InOwner)
{
    AddInputDevice(new FAirVRHeadTrackerInputDevice());
    AddInputDevice(new FAirVRLeftHandTrackerInputDevice());
    AddInputDevice(new FAirVRRightHandTrackerInputDevice());
    AddInputDevice(new FAirVRControllerInputDevice());
}

FAirVRInputStream::~FAirVRInputStream()
{
    for (auto& Device : InputDevices) {
        delete Device.Value;
    }
    InputDevices.Empty();

    for (auto& Feedback : DeviceFeedbacks) {
        delete Feedback.Value;
    }
    DeviceFeedbacks.Empty();
}

void FAirVRInputStream::Init()
{
    check(Owner->IsBound());

    for (auto& Feedback : DeviceFeedbacks) {
        Feedback.Value->OnRegistered(ocs_RegisterInputSender(Owner->GetPlayerID(), TCHAR_TO_UTF8(*Feedback.Key), Feedback.Value->GetCookieArgs()));
    }

    InputSendTimer.Set(90.0);
}

void FAirVRInputStream::Start()
{
    bStreaming = true;
}

void FAirVRInputStream::Stop()
{
    check(Owner->IsBound());
    bStreaming = false;

    ocs_ResetInput(Owner->GetPlayerID());
}

void FAirVRInputStream::Cleanup()
{
    check(Owner->IsBound());

    InputSendTimer.Reset();
    bStreaming = false;

    for (auto& Device : InputDevices) {
        Device.Value->OnUnregistered();
    }

    for (auto& Feedback : DeviceFeedbacks) {
        if (Feedback.Value->IsRegistered()) {
            ocs_UnregisterInputSender(Owner->GetPlayerID(), Feedback.Value->GetDeviceID());
            Feedback.Value->OnUnregistered();
        }
    }
}

void FAirVRInputStream::Update(FWorldContext& WorldContext)
{
    for (auto& Device : InputDevices) {
        Device.Value->PollInputsPerFrame(WorldContext, this);
    }

    InputSendTimer.UpdatePerFrame();
    if (InputSendTimer.Expired()) {
        int64_t timestamp = 0;
        ocs_BeginPendInput(Owner->GetPlayerID(), timestamp);

        for (auto& Feedback : DeviceFeedbacks) {
            if (Feedback.Value->IsRegistered()) {
                Feedback.Value->PendInputsPerFrame(WorldContext, this);
            }
        }
        ocs_SendPendingInputs(Owner->GetPlayerID(), timestamp);
    }
}

void FAirVRInputStream::Reset()
{
    InputSendTimer.Reset();
    bStreaming = false;

    for (auto& Device : InputDevices) {
        Device.Value->OnUnregistered();
    }

    for (auto& Feedback : DeviceFeedbacks) {
        Feedback.Value->OnUnregistered();
        delete Feedback.Value;
    }
    DeviceFeedbacks.Empty();
}

void FAirVRInputStream::HandleRemoteInputDeviceRegistered(const FString& DeviceName, uint8 DeviceID)
{
    if (InputDevices.Contains(DeviceName) && InputDevices[DeviceName]->IsRegistered() == false) {
        InputDevices[DeviceName]->OnRegistered(DeviceID);
    }
}

void FAirVRInputStream::HandleRemoteInputDeviceUnregistered(uint8 DeviceID)
{
    for (auto& InputDevice : InputDevices) {
        if (InputDevice.Value->IsRegistered() && InputDevice.Value->GetDeviceID() == DeviceID) {
            InputDevice.Value->OnUnregistered();
        }
    }
}

void FAirVRInputStream::GetTransform(FString DeviceName, uint8 ControlID, OCS_VECTOR3D* Position, OCS_QUATERNION* Orientation) const
{
	if (InputDevices.Contains(DeviceName)) {
		InputDevices[DeviceName]->GetTransform(ControlID, Position, Orientation);
	}
	else {
		*Position = OCS_VECTOR3D();
		*Orientation = OCS_QUATERNION();
	}
}

OCS_QUATERNION FAirVRInputStream::GetOrientation(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetOrientation(ControlID);
    }
    return OCS_QUATERNION();
}

OCS_VECTOR2D FAirVRInputStream::GetAxis2D(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetAxis2D(ControlID);
    }
    return OCS_VECTOR2D();
}

float FAirVRInputStream::GetAxis(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetAxis(ControlID);
    }
    return 0.0f;
}

float FAirVRInputStream::GetButtonRaw(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetButtonRaw(ControlID);
    }
    return 0.0f;
}

bool FAirVRInputStream::GetButton(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetButton(ControlID);
    }
    return false;
}

bool FAirVRInputStream::GetButtonDown(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetButtonDown(ControlID);
    }
    return false;
}

bool FAirVRInputStream::GetButtonUp(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetButtonUp(ControlID);
    }
    return false;
}

bool FAirVRInputStream::IsInputDeviceAvailable(const FString& DeviceName) const
{
    return InputDevices.Contains(DeviceName) && InputDevices[DeviceName]->IsRegistered();
}

bool FAirVRInputStream::IsDeviceFeedbackEnabled(const FString& DeviceName) const
{
    return DeviceFeedbacks.Contains(DeviceName);
}

void FAirVRInputStream::EnableTrackedDeviceFeedback(const FString& DeviceName, const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier)
{
    if (DeviceFeedbacks.Contains(DeviceName) == false) {
        FAirVRInputDeviceFeedback* Feedback = CreateTrackedDeviceFeedback(DeviceName, CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier);
        if (Feedback) {
            DeviceFeedbacks.Add(DeviceName, Feedback);
            if (Owner->IsBound()) {
                Feedback->OnRegistered(ocs_RegisterInputSender(Owner->GetPlayerID(), TCHAR_TO_UTF8(*DeviceName), Feedback->GetCookieArgs()));
            }
        }
    }
}

void FAirVRInputStream::DisableDeviceFeedback(const FString& DeviceName)
{
    if (DeviceFeedbacks.Contains(DeviceName)) {
        FAirVRInputDeviceFeedback* Feedback = DeviceFeedbacks[DeviceName];
        if (Owner->IsBound() && Feedback->IsRegistered()) {
            ocs_UnregisterInputSender(Owner->GetPlayerID(), Feedback->GetDeviceID());
            Feedback->OnUnregistered();
        }
        DeviceFeedbacks.Remove(DeviceName);
        delete Feedback;
    }
}

void FAirVRInputStream::EnableRaycastHit(const FString& DeviceName, bool bEnable) 
{
    if (DeviceFeedbacks.Contains(DeviceName)) {
        DeviceFeedbacks[DeviceName]->EnableRaycastHit(bEnable);
    }
}

void FAirVRInputStream::UpdateRaycastHitResult(const FString& DeviceName, const OCS_VECTOR3D& RayOrigin, const OCS_VECTOR3D& HitPosition, const OCS_VECTOR3D& HitNormal)
{
    if (DeviceFeedbacks.Contains(DeviceName)) {
        DeviceFeedbacks[DeviceName]->PendRaycastHitResult(RayOrigin, HitPosition, HitNormal);
    }
}

void FAirVRInputStream::UpdateRenderOnClient(const FString& DeviceName, bool bRenderOnClient) 
{
    if (DeviceFeedbacks.Contains(DeviceName) && DeviceFeedbacks[DeviceName]->IsControllerDevice()) {
        ((FAirVRControllerDeviceFeedback*)DeviceFeedbacks[DeviceName])->bRenderOnClient = bRenderOnClient;
    }
}

void FAirVRInputStream::PendInput(FAirVRInputSender* Sender, uint8 ControlID, const float* Values, int Length, OCS_INPUT_SENDING_POLICY Policy)
{
    check(Sender->IsRegistered());
    ocs_PendInputFloat(Owner->GetPlayerID(), static_cast<uint8>(Sender->GetDeviceID()), ControlID, Values, Length, Policy);
}

void FAirVRInputStream::PendInput(FAirVRInputSender* Sender, uint8 ControlID, const uint8* Values, int Length, OCS_INPUT_SENDING_POLICY Policy) 
{
    check(Sender->IsRegistered());
    ocs_PendInputByte(Owner->GetPlayerID(), static_cast<uint8>(Sender->GetDeviceID()), ControlID, Values, Length, Policy);
}

bool FAirVRInputStream::GetInput(FAirVRInputReceiver* Receiver, uint8 ControlID, float* Values, int Length)
{
    if (Receiver->IsRegistered()) {
        return ocs_GetInputFloat(Owner->GetPlayerID(), static_cast<uint8>(Receiver->GetDeviceID()), ControlID, Values, Length);
    }
    return false;
}

FAirVRInputDeviceFeedback* FAirVRInputStream::CreateTrackedDeviceFeedback(const FString& DeviceName, const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier) const
{
    if (DeviceName.Equals(ONAIRVR_INPUT_DEVICE_LEFT_HAND_TRACKER)) {
        return new FAirVRLeftHandTrackerDeviceFeedback(CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier);
    }
    else if (DeviceName.Equals(ONAIRVR_INPUT_DEVICE_RIGHT_HAND_TRACKER)) {
        return new FAirVRRightHandTrackerDeviceFeedback(CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier);
    }
    return nullptr;
}

void FAirVRInputStream::AddInputDevice(FAirVRInputDevice* Device)
{
    InputDevices.Add(Device->Name(), Device);
}
