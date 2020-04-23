/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRInputDeviceFeedback.h"
#include "AirVRServerPrivate.h"

#define MIN_VIBRATE_REQUEST_INTERVAL 0.06f

FAirVRInputDeviceFeedback::FAirVRInputDeviceFeedback()
    : bSupportsRaycastHit(false), 
      LastVibration(AirVRHapticVibration::None),
      RemainingToResetVibration(-1.0f)
{
    CookieArgs.pointerCookieTexture = nullptr;
}

FAirVRInputDeviceFeedback::~FAirVRInputDeviceFeedback() 
{
    if (CookieArgs.pointerCookieTexture) {
        delete CookieArgs.pointerCookieTexture;
    }
}

void FAirVRInputDeviceFeedback::ConfigureRaycastHit(const void* InCookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier) {
    if (CookieArgs.pointerCookieTexture) {
        delete CookieArgs.pointerCookieTexture;
    }

    uint8* CookieTexture = new uint8[CookieTextureSize];
    FMemory::Memcpy(CookieTexture, InCookieTexture, CookieTextureSize);

    CookieArgs.pointerCookieTexture = CookieTexture;
    CookieArgs.pointerCookieTextureSize = CookieTextureSize;
    CookieArgs.pointerCookieDepthScaleMultiplier = CookieDepthScaleMultiplier;
}

void FAirVRInputDeviceFeedback::EnableRaycastHit(bool bEnable) 
{
    if (!CookieArgs.pointerCookieTexture || 
        CookieArgs.pointerCookieDepthScaleMultiplier <= 0.0f ||
        bSupportsRaycastHit == bEnable) { return; }
    
    bSupportsRaycastHit = bEnable;

    if (!bEnable) {
        RayOrigin = OCS_VECTOR3D();
        HitPosition = OCS_VECTOR3D();
        HitNormal = OCS_VECTOR3D();
    }
}

void FAirVRInputDeviceFeedback::PendRaycastHitResult(const OCS_VECTOR3D& InRayOrigin, const OCS_VECTOR3D& InHitPosition, const OCS_VECTOR3D& InHitNormal) 
{
    if (!bSupportsRaycastHit) { return; }

    RayOrigin = InRayOrigin;
    HitPosition = InHitPosition;
    HitNormal = InHitNormal;
}

void FAirVRInputDeviceFeedback::PendVibrate(AirVRHapticVibration Vibration) 
{
    if (!SupportsVibrate()) { return; }

    LastVibration = Vibration;
    if (LastVibration != AirVRHapticVibration::None) {
        RemainingToResetVibration = MIN_VIBRATE_REQUEST_INTERVAL;
    }
}

void FAirVRInputDeviceFeedback::PendInputsPerFrame(FWorldContext& WorldContext, FAirVRInputStream* InputStream) 
{    
    if (bSupportsRaycastHit) {
        PendRaycastHitPerFrame(InputStream);
    }
    if (SupportsVibrate()) {
        PendVibrationPerFrame(WorldContext, InputStream);
    }
}

void FAirVRInputDeviceFeedback::PendRaycastHitPerFrame(FAirVRInputStream* InputStream) 
{
    float Values[9] = { RayOrigin.y, RayOrigin.z, RayOrigin.x,
                        HitPosition.y, HitPosition.z, HitPosition.x,
                        HitNormal.y, HitNormal.z, HitNormal.x };

    InputStream->PendInput(this, RaycastHitResultControlID(), Values, 9, OCS_INPUT_SENDING_POLICY_ALWAYS);
}

void FAirVRInputDeviceFeedback::PendVibrationPerFrame(FWorldContext& WorldContext, FAirVRInputStream* InputStream) 
{
    if (RemainingToResetVibration > 0.0f) {
        RemainingToResetVibration -= UGameplayStatics::GetWorldDeltaSeconds(WorldContext.World());

        if (RemainingToResetVibration <= 0.0f) {
            LastVibration = AirVRHapticVibration::None;
        }
    }

    uint8 Vibration = static_cast<uint8>(LastVibration);
    InputStream->PendInput(this, VibrateControlID(), &Vibration, 1, OCS_INPUT_SENDING_POLICY_ON_CHANGE);
}

FAirVRControllerDeviceFeedback::FAirVRControllerDeviceFeedback(const void* CookieTexture, int CookieTextureSize, float CookieDepthScaleMultiplier)
    : bRenderOnClient(false)
{
    ConfigureRaycastHit(CookieTexture, CookieTextureSize, CookieDepthScaleMultiplier);
}

void FAirVRControllerDeviceFeedback::PendInputsPerFrame(FWorldContext& WorldContext, FAirVRInputStream* InputStream)
{
    FAirVRInputDeviceFeedback::PendInputsPerFrame(WorldContext, InputStream);

    uint8 RenderOnClient = bRenderOnClient ? 1 : 0;
    InputStream->PendInput(this, RenderOnClientControlID(), &RenderOnClient, 1, OCS_INPUT_SENDING_POLICY_ON_CHANGE);
}
