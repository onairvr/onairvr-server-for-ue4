/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Engine/Engine.h"
#include "FixedRateTimer.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "onairvr_input.h"
#include "ocs_server.h"
#include "Windows/HideWindowsPlatformTypes.h"

class FAirVRInputBase
{
public:
    virtual uint8 ID() const = 0;
};

class FAirVRInputSender  : public FAirVRInputBase
{
public:
    virtual void PendInputsPerFrame(FWorldContext& WorldContext, class FAirVRInputStream* InputStream) = 0;
};

class FAirVRInputStream {
public:
    FAirVRInputStream(class FAirVRCameraRig* InOwner);
    ~FAirVRInputStream();

public:
    void Init();
    void Start();
    void Stop();
    void Cleanup();
    void Update(FWorldContext& WorldContext);
    void Reset();

    void PendState(AirVRInputDeviceID Device, uint8 Control, uint8 State) const;
    void PendRaycastHit(AirVRInputDeviceID Device, uint8 Control, const OCS_VECTOR3D& Origin, const OCS_VECTOR3D& HitPosition, const OCS_VECTOR3D& HitNormal) const;
    void PendVibration(AirVRInputDeviceID Device, uint8 Control, float Frequency, float Amplitude) const;

    bool GetState(AirVRInputDeviceID Device, uint8 Control, uint8* State) const;
    bool GetByteAxis(AirVRInputDeviceID Device, uint8 Control, uint8* Axis) const;
    bool GetAxis(AirVRInputDeviceID Device, uint8 Control, float* Axis) const;
    bool GetAxis2D(AirVRInputDeviceID Device, uint8 Control, OCS_VECTOR2D* Axis2D) const;
    bool GetPose(AirVRInputDeviceID Device, uint8 Control, OCS_VECTOR3D* Position, OCS_QUATERNION* Rotation) const;
    bool GetTouch2D(AirVRInputDeviceID Device, uint8 Control, OCS_VECTOR2D* Position, uint8* State) const;
    bool IsActive(AirVRInputDeviceID Device, uint8 Control) const;
    bool IsActive(AirVRInputDeviceID Device, uint8_t Control, OCS_INPUT_DIRECTION Direction) const;
    bool GetActivated(AirVRInputDeviceID Device, uint8_t Control) const;
    bool GetActivated(AirVRInputDeviceID Device, uint8_t Control, OCS_INPUT_DIRECTION Direction) const;
    bool GetDeactivated(AirVRInputDeviceID Device, uint8_t Control) const;
    bool GetDeactivated(AirVRInputDeviceID Device, uint8_t Control, OCS_INPUT_DIRECTION Direction) const;

private:
    void RegisterInputSender(FAirVRInputSender* Sender);

private:
    class FAirVRCameraRig* Owner;
    bool bStreaming;
    FFixedRateTimer InputSendTimer;

    TMap<uint8, FAirVRInputSender*> Senders;
};
