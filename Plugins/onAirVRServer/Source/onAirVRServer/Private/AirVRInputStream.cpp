/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRInputStream.h"
#include "AirVRServerPrivate.h"

#include "AirVRCameraRig.h"
#include "AirVRHeadTrackerInputDevice.h"
#include "AirVRTouchpadInputDevice.h"
#include "AirVRGamepadInputDevice.h"
#include "AirVRTrackedControllerInputDevice.h"
#include "AirVRLeftControllerInputDevice.h"
#include "AirVRRightControllerInputDevice.h"
#include "AirVRDeviceFeedback.h"

FAirVRInputStream::FAirVRInputStream(FAirVRCameraRig* InOwner)
    : Owner(InOwner)
{
    AddInputDevice(new FAirVRHeadTrackerInputDevice());
    AddInputDevice(new FAirVRTouchpadInputDevice());
    AddInputDevice(new FAirVRGamepadInputDevice());
    AddInputDevice(new FAirVRTrackedControllerInputDevice());
    AddInputDevice(new FAirVRLeftControllerInputDevice());
    AddInputDevice(new FAirVRRightControllerInputDevice());
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
        Feedback.Value->OnRegistered(onairvr_RegisterInputSender(Owner->GetPlayerID(), TCHAR_TO_UTF8(*Feedback.Key), Feedback.Value->GetCookieArgs()));
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

    onairvr_ResetInput(Owner->GetPlayerID());
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
            onairvr_UnregisterInputSender(Owner->GetPlayerID(), Feedback.Value->GetDeviceID());
            Feedback.Value->OnUnregistered();
        }
    }
}

void FAirVRInputStream::Update()
{
    for (auto& Device : InputDevices) {
        Device.Value->PollInputsPerFrame(this);
    }

    InputSendTimer.UpdatePerFrame();
    if (InputSendTimer.Expired()) {
        for (auto& Feedback : DeviceFeedbacks) {
            if (Feedback.Value->IsRegistered()) {
                Feedback.Value->PendInputsPerFrame(this);
            }
        }
        onairvr_SendPendingInputs(Owner->GetPlayerID());
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

void FAirVRInputStream::GetTransform(FString DeviceName, uint8 ControlID, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation) const
{
	if (InputDevices.Contains(DeviceName)) {
		InputDevices[DeviceName]->GetTransform(ControlID, Position, Orientation);
	}
	else {
		*Position = ONAIRVR_VECTOR3D();
		*Orientation = ONAIRVR_QUATERNION();
	}
}

void FAirVRInputStream::GetTransform(FString DeviceName, uint8 ControlID, double& TimeStamp, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation) const
{
    if (InputDevices.Contains(DeviceName)) {
        InputDevices[DeviceName]->GetTransform(ControlID, TimeStamp, Position, Orientation);
    }
    else {
		TimeStamp = 0.0;
        *Position = ONAIRVR_VECTOR3D();
        *Orientation = ONAIRVR_QUATERNION();
    }
}

ONAIRVR_QUATERNION FAirVRInputStream::GetOrientation(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetOrientation(ControlID);
    }
    return ONAIRVR_QUATERNION();
}

ONAIRVR_VECTOR2D FAirVRInputStream::GetAxis2D(const FString& DeviceName, uint8 ControlID) const
{
    if (InputDevices.Contains(DeviceName)) {
        return InputDevices[DeviceName]->GetAxis2D(ControlID);
    }
    return ONAIRVR_VECTOR2D();
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
        FAirVRTrackedDeviceFeedback* Feedback = CreateTrackedDeviceFeedback(DeviceName, CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier);
        if (Feedback) {
            DeviceFeedbacks.Add(DeviceName, Feedback);
            if (Owner->IsBound()) {
                Feedback->OnRegistered(onairvr_RegisterInputSender(Owner->GetPlayerID(), TCHAR_TO_UTF8(*DeviceName), Feedback->GetCookieArgs()));
            }
        }
    }
}

void FAirVRInputStream::DisableDeviceFeedback(const FString& DeviceName)
{
    if (DeviceFeedbacks.Contains(DeviceName)) {
        FAirVRTrackedDeviceFeedback* Feedback = DeviceFeedbacks[DeviceName];
        if (Owner->IsBound() && Feedback->IsRegistered()) {
            onairvr_UnregisterInputSender(Owner->GetPlayerID(), Feedback->GetDeviceID());
            Feedback->OnUnregistered();
        }
        DeviceFeedbacks.Remove(DeviceName);
        delete Feedback;
    }
}

void FAirVRInputStream::FeedbackTrackedDevice(const FString& DeviceName, uint8 ControlID, const ONAIRVR_VECTOR3D& RayOrigin, const ONAIRVR_VECTOR3D& HitPosition, const ONAIRVR_VECTOR3D& HitNormal)
{
    if (DeviceFeedbacks.Contains(DeviceName)) {
        DeviceFeedbacks[DeviceName]->SetFeedback(RayOrigin, HitPosition, HitNormal);
    }
}

void FAirVRInputStream::PendInput(FAirVRInputSender* Sender, uint8 ControlID, const float* Values, int Length, ONAIRVR_INPUT_SENDING_POLICY Policy)
{
    check(Sender->IsRegistered());
    onairvr_PendInput(Owner->GetPlayerID(), static_cast<uint8>(Sender->GetDeviceID()), ControlID, Values, Length, Policy);
}

bool FAirVRInputStream::GetInput(FAirVRInputReceiver* Receiver, uint8 ControlID, float* Values, int Length)
{
    if (Receiver->IsRegistered()) {
        return onairvr_GetInput(Owner->GetPlayerID(), static_cast<uint8>(Receiver->GetDeviceID()), ControlID, Values, Length);
    }
    return false;
}

bool FAirVRInputStream::GetInputWithTimeStamp(FAirVRInputReceiver* Receiver, uint8 ControlID, float* Values, int Length, double* TimeStamp)
{
	if (Receiver->IsRegistered()) {
		return onairvr_GetInput(Owner->GetPlayerID(), static_cast<uint8>(Receiver->GetDeviceID()), ControlID, Values, Length, TimeStamp);
	}
	return false;
}

FAirVRTrackedDeviceFeedback* FAirVRInputStream::CreateTrackedDeviceFeedback(const FString& DeviceName, const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier) const
{
    if (DeviceName.Equals(ONAIRVR_INPUT_DEVICE_HEADTRACKER)) {
        return new FAirVRHeadTrackerDeviceFeedback(CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier);
    }
    else if (DeviceName.Equals(ONAIRVR_INPUT_DEVICE_TRACKED_CONTROLLER)) {
        return new FAirVRTrackedControllerDeviceFeedback(CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier);
    }
    return nullptr;
}

void FAirVRInputStream::AddInputDevice(FAirVRInputDevice* Device)
{
    InputDevices.Add(Device->Name(), Device);
}
