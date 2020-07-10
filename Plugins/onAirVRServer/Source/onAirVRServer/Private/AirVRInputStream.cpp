/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRInputStream.h"
#include "AirVRServerPrivate.h"

#include "AirVRCameraRig.h"

FAirVRInputStream::FAirVRInputStream(FAirVRCameraRig* InOwner)
    : Owner(InOwner)
{}

FAirVRInputStream::~FAirVRInputStream()
{
    for (auto& Sender : Senders) {
        delete Sender.Value;
    }
    Senders.Empty();
}

void FAirVRInputStream::Init()
{
    check(Owner->IsBound());

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

    ocs_ClearInput(Owner->GetPlayerID());
}

void FAirVRInputStream::Cleanup()
{
    check(Owner->IsBound());

    InputSendTimer.Reset();
    bStreaming = false;

    ocs_ClearInput(Owner->GetPlayerID());
}

void FAirVRInputStream::Update(FWorldContext& WorldContext)
{
    if (Owner->IsBound() == false) { return; }

    ocs_UpdateInputFrame(Owner->GetPlayerID());

    InputSendTimer.UpdatePerFrame();
    if (InputSendTimer.Expired()) {
        int64_t timestamp = 0;
        ocs_BeginPendInput(Owner->GetPlayerID(), timestamp);

        for (auto& Sender : Senders) {
            Sender.Value->PendInputsPerFrame(WorldContext, this);
        }
        ocs_SendPendingInputs(Owner->GetPlayerID(), timestamp);
    }
}

void FAirVRInputStream::Reset()
{
    InputSendTimer.Reset();
    bStreaming = false;
}

void FAirVRInputStream::PendState(AirVRInputDeviceID Device, uint8 Control, uint8 State) const
{
    if (Owner->IsBound() == false) { return; }

    ocs_PendInputState(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, State);
}

void FAirVRInputStream::PendRaycastHit(AirVRInputDeviceID Device, uint8 Control, const OCS_VECTOR3D& Origin, const OCS_VECTOR3D& HitPosition, const OCS_VECTOR3D& HitNormal) const
{
    if (Owner->IsBound() == false) { return; }

    ocs_PendInputRaycastHit(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Origin, HitPosition, HitNormal);
}

void FAirVRInputStream::PendVibration(AirVRInputDeviceID Device, uint8 Control, float Frequency, float Amplitude) const 
{
    if (Owner->IsBound() == false) { return; }

    ocs_PendInputVibration(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Frequency, Amplitude);
}

bool FAirVRInputStream::GetState(AirVRInputDeviceID Device, uint8 Control, uint8* State) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputState(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, State);
}

bool FAirVRInputStream::GetByteAxis(AirVRInputDeviceID Device, uint8 Control, uint8* Axis) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputByteAxis(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Axis);
}

bool FAirVRInputStream::GetAxis(AirVRInputDeviceID Device, uint8 Control, float* Axis) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputAxis(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Axis);
}

bool FAirVRInputStream::GetAxis2D(AirVRInputDeviceID Device, uint8 Control, OCS_VECTOR2D* Axis2D) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputAxis2D(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Axis2D);
}

bool FAirVRInputStream::GetPose(AirVRInputDeviceID Device, uint8 Control, OCS_VECTOR3D* Position, OCS_QUATERNION* Rotation) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputPose(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Position, Rotation);
}

bool FAirVRInputStream::GetTouch2D(AirVRInputDeviceID Device, uint8 Control, OCS_VECTOR2D* Position, uint8* State) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputTouch2D(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Position, State);
}

bool FAirVRInputStream::IsActive(AirVRInputDeviceID Device, uint8 Control) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_IsInputActive(Owner->GetPlayerID(), static_cast<uint8>(Device), Control);
}

bool FAirVRInputStream::IsActive(AirVRInputDeviceID Device, uint8_t Control, OCS_INPUT_DIRECTION Direction) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_IsInputDirectionActive(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Direction);
}

bool FAirVRInputStream::GetActivated(AirVRInputDeviceID Device, uint8_t Control) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputActivated(Owner->GetPlayerID(), static_cast<uint8>(Device), Control);
}

bool FAirVRInputStream::GetActivated(AirVRInputDeviceID Device, uint8_t Control, OCS_INPUT_DIRECTION Direction) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputDirectionActivated(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Direction);
}

bool FAirVRInputStream::GetDeactivated(AirVRInputDeviceID Device, uint8_t Control) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputDeactivated(Owner->GetPlayerID(), static_cast<uint8>(Device), Control);
}

bool FAirVRInputStream::GetDeactivated(AirVRInputDeviceID Device, uint8_t Control, OCS_INPUT_DIRECTION Direction) const 
{
    if (Owner->IsBound() == false) { return false; }

    return ocs_GetInputDirectionDeactivated(Owner->GetPlayerID(), static_cast<uint8>(Device), Control, Direction);
}

void FAirVRInputStream::RegisterInputSender(FAirVRInputSender* Sender) 
{
    Senders.Add(Sender->ID(), Sender);
}
