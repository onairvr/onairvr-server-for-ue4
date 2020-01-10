/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRDeviceFeedback.h"
#include "AirVRServerPrivate.h"

FAirVRTrackedDeviceFeedback::FAirVRTrackedDeviceFeedback(const void* InCookieTexture, int InCookieTextureSize, float InCookieDepthScaleMultiplier)
{
    uint8* CookieTexture = new uint8[InCookieTextureSize];
    FMemory::Memcpy(CookieTexture, InCookieTexture, InCookieTextureSize);

    CookieArgs.pointerCookieTexture = CookieTexture;
    CookieArgs.pointerCookieTextureSize = InCookieTextureSize;
    CookieArgs.pointerCookieDepthScaleMultiplier = InCookieDepthScaleMultiplier;
}

FAirVRTrackedDeviceFeedback::~FAirVRTrackedDeviceFeedback()
{
    delete CookieArgs.pointerCookieTexture;
}

void FAirVRTrackedDeviceFeedback::SetFeedback(const ONAIRVR_VECTOR3D& InRayOrigin, const ONAIRVR_VECTOR3D& InHitPosition, const ONAIRVR_VECTOR3D& InHitNormal)
{
    RayOrigin = InRayOrigin;
    HitPosition = InHitPosition;
    HitNormal = InHitNormal;
}

void FAirVRTrackedDeviceFeedback::PendInputsPerFrame(FAirVRInputStream* InputStream)
{
    float Values[9] = { RayOrigin.y, RayOrigin.z, RayOrigin.x,
                        HitPosition.y, HitPosition.z, HitPosition.x,
                        HitNormal.y, HitNormal.z, HitNormal.x };

    InputStream->PendInput(this, RaycastHitResultControlID(), Values, 9, ONAIRVR_INPUT_SENDING_POLICY_ALWAYS);
}
