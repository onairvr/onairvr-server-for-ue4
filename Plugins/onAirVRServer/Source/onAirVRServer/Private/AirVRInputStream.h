/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "FixedRateTimer.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "onairvr_input.h"
#include "onairvr_server.h"
#include "Windows/HideWindowsPlatformTypes.h"

class FAirVRInputBase
{
public:
    static const int16 InvalidDeviceID = -1;

public:
    FAirVRInputBase() : DeviceID(InvalidDeviceID) {}

public:
    bool IsRegistered() const           { return DeviceID != InvalidDeviceID; }

    int16 GetDeviceID() const           { return DeviceID; }
    void OnRegistered(uint8 InDeviceID) { DeviceID = static_cast<int16>(InDeviceID); }
    void OnUnregistered()               { DeviceID = InvalidDeviceID; }

    virtual FString Name() const = 0;

private:
    int16 DeviceID;
};

class FAirVRInputSender  : public FAirVRInputBase
{
public:
    virtual void PendInputsPerFrame(class FAirVRInputStream* InputStream) = 0;
};

class FAirVRInputReceiver : public FAirVRInputBase
{
public:
    virtual void PollInputsPerFrame(class FAirVRInputStream* InputStream) = 0;
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
    void Update();
    void Reset();

    void HandleRemoteInputDeviceRegistered(const FString& DeviceName, uint8 DeviceID);
    void HandleRemoteInputDeviceUnregistered(uint8 DeviceID);

	void GetTransform(FString DeviceName, uint8 ControlID, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation) const;
    void GetTransform(FString DeviceName, uint8 ControlID, double& TimeStamp, ONAIRVR_VECTOR3D* Position, ONAIRVR_QUATERNION* Orientation) const;
    ONAIRVR_QUATERNION GetOrientation(const FString& DeviceName, uint8 ControlID) const;
    ONAIRVR_VECTOR2D GetAxis2D(const FString& DeviceName, uint8 ControlID) const;
    float GetAxis(const FString& DeviceName, uint8 ControlID) const;
    float GetButtonRaw(const FString& DeviceName, uint8 ControlID) const;
    bool GetButton(const FString& DeviceName, uint8 ControlID) const;
    bool GetButtonDown(const FString& DeviceName, uint8 ControlID) const;
    bool GetButtonUp(const FString& DeviceName, uint8 ControlID) const;

    bool IsInputDeviceAvailable(const FString& DeviceName) const;
    bool IsDeviceFeedbackEnabled(const FString& DeviceName) const;
    void EnableTrackedDeviceFeedback(const FString& DeviceName, const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier);
    void DisableDeviceFeedback(const FString& DeviceName);
    void FeedbackTrackedDevice(const FString& DeviceName, uint8 ControlID, const ONAIRVR_VECTOR3D& RayOrigin, const ONAIRVR_VECTOR3D& HitPosition, const ONAIRVR_VECTOR3D& HitNormal);

public:
    // for senders and receivers
    void PendInput(FAirVRInputSender* Sender, uint8 ControlID, const float* Values, int Length, ONAIRVR_INPUT_SENDING_POLICY Policy);
    bool GetInput(FAirVRInputReceiver* Receiver, uint8 ControlID, float* Values, int Length);
	bool GetInputWithTimeStamp(FAirVRInputReceiver* Receiver, uint8 ControlID, float* Values, int Length, double* TimeStamp);

private:
    class FAirVRTrackedDeviceFeedback* CreateTrackedDeviceFeedback(const FString& DeviceName, const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier) const;

    void AddInputDevice(class FAirVRInputDevice* Device);

private:
    class FAirVRCameraRig* Owner;
    bool bStreaming;
    FFixedRateTimer InputSendTimer;

    TMap<FString, class FAirVRInputDevice*> InputDevices;
    TMap<FString, class FAirVRTrackedDeviceFeedback*> DeviceFeedbacks;
};
