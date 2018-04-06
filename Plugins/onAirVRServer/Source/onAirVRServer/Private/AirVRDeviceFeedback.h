/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputStream.h"

class FAirVRDeviceFeedback : public FAirVRInputSender {};

class FAirVRTrackedDeviceFeedback : public FAirVRDeviceFeedback
{
public:
    FAirVRTrackedDeviceFeedback(const void* InCookieTexture, int InCookieTextureSize, float InCookieDepthScaleMultiplier);
    virtual ~FAirVRTrackedDeviceFeedback();

public:
    const ONAIRVR_INPUT_SENDER_ARGS* GetCookieArgs() const  { return &CookieArgs; }

private:
    ONAIRVR_INPUT_SENDER_ARGS CookieArgs;

    ONAIRVR_VECTOR3D RayOrigin;
    ONAIRVR_VECTOR3D HitPosition;
    ONAIRVR_VECTOR3D HitNormal;

protected:
    virtual uint8 RaycastHitResultControlID() const = 0;

public:
    void SetFeedback(const ONAIRVR_VECTOR3D& InRayOrigin, const ONAIRVR_VECTOR3D& InHitPosition, const ONAIRVR_VECTOR3D& InHitNormal);

    // implements FAirVRInputDeviceFeedback
    virtual void PendInputsPerFrame(class FAirVRInputStream* InputStream) override;
};

class FAirVRHeadTrackerDeviceFeedback : public FAirVRTrackedDeviceFeedback
{
public:
    FAirVRHeadTrackerDeviceFeedback(const void* InCookieTexture, int InCookieTextureSize, float InCookieDepthScaleMultiplier)
        : FAirVRTrackedDeviceFeedback(InCookieTexture, InCookieTextureSize, InCookieDepthScaleMultiplier) {}

protected:
    // implements FAirVRPointerInputDeviceFeedback
    virtual uint8 RaycastHitResultControlID() const override { return (uint8)AirVRHeadTrackerKey::RaycastHitResult; }

public:
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_HEADTRACKER; }
};

class FAirVRTrackedControllerDeviceFeedback : public FAirVRTrackedDeviceFeedback
{
public:
    FAirVRTrackedControllerDeviceFeedback(const void* InCookieTexture, int InCookieTextureSize, float InCookieDepthScaleMultiplier)
        : FAirVRTrackedDeviceFeedback(InCookieTexture, InCookieTextureSize, InCookieDepthScaleMultiplier) {}

protected:
    // implements FAirVRPointerInputDeviceFeedback
    virtual uint8 RaycastHitResultControlID() const override { return (uint8)AirVRTrackedControllerKey::RaycastHitResult; }

public:
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_TRACKED_CONTROLLER; }
};
