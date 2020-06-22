/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputStream.h"

class FAirVRInputDeviceFeedback : public FAirVRInputSender {
public:
    FAirVRInputDeviceFeedback();
    virtual ~FAirVRInputDeviceFeedback();

public:
    const OCS_INPUT_SENDER_ARGS* GetCookieArgs() const  { return &CookieArgs; }
    virtual bool IsControllerDevice() const             { return false; }

    void ConfigureRaycastHit(const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier);
    void EnableRaycastHit(bool bEnable);
    void PendRaycastHitResult(const OCS_VECTOR3D& InRayOrigin, const OCS_VECTOR3D& InHitPosition, const OCS_VECTOR3D& InHitNormal);

    void PendVibrate(AirVRHapticVibration Vibration);

    // implements FAirVRInputSender
    virtual void PendInputsPerFrame(FWorldContext& WorldContext, FAirVRInputStream* InputStream) override;

protected:
    virtual uint8 RaycastHitResultControlID() const { return 0; }

    virtual bool SupportsVibrate() const = 0;
    virtual uint8 VibrateControlID() const { return 0; }

private:
    void PendRaycastHitPerFrame(FAirVRInputStream* InputStream);
    void PendVibrationPerFrame(FWorldContext& WorldContext, FAirVRInputStream* InputStream);

protected:
    bool bSupportsRaycastHit;

private:
    OCS_INPUT_SENDER_ARGS CookieArgs;

    OCS_VECTOR3D RayOrigin;
    OCS_VECTOR3D HitPosition;
    OCS_VECTOR3D HitNormal;

    AirVRHapticVibration LastVibration;
    float RemainingToResetVibration;
};

class FAirVRHeadTrackerDeviceFeedback : public FAirVRInputDeviceFeedback 
{
public:
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_HEADTRACKER; }

protected:
    virtual uint8 RaycastHitResultControlID() const override { return (uint8)AirVRHeadTrackerKey::RaycastHitResult; }
    virtual bool SupportsVibrate() const override { return false; }
};

class FAirVRControllerDeviceFeedback : public FAirVRInputDeviceFeedback 
{
public:
    FAirVRControllerDeviceFeedback(const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier);

public:
    bool bRenderOnClient;

public:
    virtual uint8 RenderOnClientControlID() const = 0;

    // overrides FAirVRInputDeviceFeedback
    virtual bool IsControllerDevice() const { return true; }
    virtual void PendInputsPerFrame(FWorldContext& WorldContext, FAirVRInputStream* InputStream) override;

protected:
    // overrides FAirVRInputDeviceFeedback
    virtual bool SupportsVibrate() const { return true; }
};

class FAirVRLeftHandTrackerDeviceFeedback : public FAirVRControllerDeviceFeedback
{
public:
    FAirVRLeftHandTrackerDeviceFeedback(const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier)
        : FAirVRControllerDeviceFeedback(CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier) {}

public:
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_LEFT_HAND_TRACKER; }

protected:
    virtual uint8 RaycastHitResultControlID() const override    { return (uint8)AirVRLeftHandTrackerKey::RaycastHitResult; }
    virtual uint8 VibrateControlID() const override             { return (uint8)AirVRLeftHandTrackerKey::Vibrate; }
    virtual uint8 RenderOnClientControlID() const override      { return (uint8)AirVRLeftHandTrackerKey::RenderOnClient; }
};

class FAirVRRightHandTrackerDeviceFeedback : public FAirVRControllerDeviceFeedback
{
public:
    FAirVRRightHandTrackerDeviceFeedback(const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier) 
        : FAirVRControllerDeviceFeedback(CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier) {}

public:
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_RIGHT_HAND_TRACKER; }

protected:
    virtual uint8 RaycastHitResultControlID() const override    { return (uint8)AirVRRightHandTrackerKey::RaycastHitResult; }
    virtual uint8 VibrateControlID() const override             { return (uint8)AirVRRightHandTrackerKey::Vibrate; }
    virtual uint8 RenderOnClientControlID() const override      { return (uint8)AirVRRightHandTrackerKey::RenderOnClient; }
};
