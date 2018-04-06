/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRInputDevice::FAirVRInputDevice()
{}

FAirVRInputDevice::~FAirVRInputDevice()
{
    for (auto Control : Controls) {
        delete Control.Value;
    }

    for (auto Control : ExtControls) {
        delete Control.Value;
    }
}

void FAirVRInputDevice::AddControlTouch(uint8 ControlID)
{
    Controls.Add(ControlID, new Touch());
}

void FAirVRInputDevice::AddControlTransform(uint8 ControlID)
{
    Controls.Add(ControlID, new Transform());
}

void FAirVRInputDevice::AddControlOrientation(uint8 ControlID)
{
    Controls.Add(ControlID, new Orientation());
}

void FAirVRInputDevice::AddControlAxis3D(uint8 ControlID)
{
    Controls.Add(ControlID, new Axis3D());
}

void FAirVRInputDevice::AddControlAxis2D(uint8 ControlID)
{
    Controls.Add(ControlID, new Axis2D());
}

void FAirVRInputDevice::AddControlAxis(uint8 ControlID)
{
    Controls.Add(ControlID, new Axis());
}

void FAirVRInputDevice::AddControlButton(uint8 ControlID)
{
    Controls.Add(ControlID, new Button());
}

void FAirVRInputDevice::AddExtControlAxis2D(uint8 ControlID)
{
    ExtControls.Add(ControlID, new Axis2D());
}

void FAirVRInputDevice::AddExtControlButton(uint8 ControlID)
{
    ExtControls.Add(ControlID, new Button());
}

void FAirVRInputDevice::SetExtControlAxis2D(uint8 ControlID, const ONAIRVR_VECTOR2D& Value)
{
    check(ExtControls.Contains(ControlID));
    ExtControls[ControlID]->AsAxis2D()->SetValue(Value);
}

void FAirVRInputDevice::SetExtControlButton(uint8 ControlID, float Value)
{
    check(ExtControls.Contains(ControlID));
    ExtControls[ControlID]->AsButton()->SetValue(Value);
}

void FAirVRInputDevice::GetTouch(uint8 ControlID, ONAIRVR_VECTOR2D* Position, bool* bTouch)
{
    Control* ControlObject = FindControl(ControlID);
    if (ControlObject) {
        *Position = ControlObject->AsTouch()->Position();
        *bTouch = ControlObject->AsTouch()->IsTouched();
    }
    else {
        *Position = ONAIRVR_VECTOR2D();
        *bTouch = false;
    }
}

void FAirVRInputDevice::GetTransform(uint8 ControlID, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation)
{
	Control* ControlObject = FindControl(ControlID);
	if (ControlObject) {
		*Position = ControlObject->AsTransform()->Position();
		*Orientation = ControlObject->AsTransform()->Orientation();
	}
	else {
		*Position = ONAIRVR_VECTOR3D();
		*Orientation = ONAIRVR_QUATERNION();
	}
}

void FAirVRInputDevice::GetTransform(uint8 ControlID, double& TimeStamp, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation)
{
    Control* ControlObject = FindControl(ControlID);
    if (ControlObject) {
		TimeStamp = ControlObject->AsTransform()->TimeStamp();
        *Position = ControlObject->AsTransform()->Position();
        *Orientation = ControlObject->AsTransform()->Orientation();
    }
    else {
		TimeStamp = 0.0;
        *Position = ONAIRVR_VECTOR3D();
        *Orientation = ONAIRVR_QUATERNION();
    }
}

ONAIRVR_QUATERNION FAirVRInputDevice::GetOrientation(uint8 ControlID)
{
    Control* ControlObject = FindControl(ControlID);
    return ControlObject ? ControlObject->AsOrientation()->Value() : ONAIRVR_QUATERNION();
}

ONAIRVR_VECTOR3D FAirVRInputDevice::GetAxis3D(uint8 ControlID)
{
    Control* ControlObject = FindControl(ControlID);
    return ControlObject ? ControlObject->AsAxis3D()->Value() : ONAIRVR_VECTOR3D();
}

ONAIRVR_VECTOR2D FAirVRInputDevice::GetAxis2D(uint8 ControlID)
{
    Control* ControlObject = FindControl(ControlID);
    return ControlObject ? ControlObject->AsAxis2D()->Value() : ONAIRVR_VECTOR2D();
}

float FAirVRInputDevice::GetAxis(uint8 ControlID)
{
    Control* ControlObject = FindControl(ControlID);
    return ControlObject ? ControlObject->AsAxis()->Value() : 0.0f;
}

float FAirVRInputDevice::GetButtonRaw(uint8 ControlID)
{
    Control* ControlObject = FindControl(ControlID);
    return ControlObject ? ControlObject->AsButton()->Value() : 0.0f;
}

bool FAirVRInputDevice::GetButton(uint8 ControlID)
{
    return GetButtonRaw(ControlID) != 0.0f;
}

bool FAirVRInputDevice::GetButtonDown(uint8 ControlID)
{
    Control* ControlObject = FindControl(ControlID);
    return ControlObject ? ControlObject->AsButton()->IsDown() : false;
}

bool FAirVRInputDevice::GetButtonUp(uint8 ControlID)
{
    Control* ControlObject = FindControl(ControlID);
    return ControlObject ? ControlObject->AsButton()->IsUp() : false;
}

void FAirVRInputDevice::PollInputsPerFrame(FAirVRInputStream* InputStream)
{
    for (auto& Control : Controls) {
        Control.Value->PollInput(this, InputStream, Control.Key);
    }
    UpdateExtendedControls();
}

FAirVRInputDevice::Control* FAirVRInputDevice::FindControl(uint8 ControlID)
{
    return Controls.Contains(ControlID) ? Controls[ControlID] :
           ExtControls.Contains(ControlID) ? ExtControls[ControlID] : nullptr;
}
