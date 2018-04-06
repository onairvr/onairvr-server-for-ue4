/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRClientConfigImpl.h"
#include "AirVRServerPrivate.h"

static EAirVRClientType parseAirVRClientType(ONAIRVR_CLIENT_TYPE type) 
{
    switch (type) {
    case ONAIRVR_CLIENT_NONE:
        return EAirVRClientType::None;
    case ONAIRVR_CLIENT_MONOSCOPIC:
        return EAirVRClientType::Monoscopic;
    case ONAIRVR_CLIENT_STEREOSCOPIC:
        return EAirVRClientType::Stereoscopic;
    default:
        break;
    }
    check(false);
    return EAirVRClientType::None;
}

static ONAIRVR_CLIENT_TYPE parseAirVRClientType(EAirVRClientType type) {
    switch (type) {
    case EAirVRClientType::None:
        return ONAIRVR_CLIENT_NONE;
    case EAirVRClientType::Monoscopic:
        return ONAIRVR_CLIENT_MONOSCOPIC;
    case EAirVRClientType::Stereoscopic:
        return ONAIRVR_CLIENT_STEREOSCOPIC;
    default:
        break;
    }
    check(false);
    return ONAIRVR_CLIENT_NONE;
}

UAirVRClientConfigImpl::UAirVRClientConfigImpl()
{
    ClientType = EAirVRClientType::None;
    VideoWidth = VideoHeight = 0;
    FrameRate = IPD = 0.0f;
    EyeCenterPosition = FVector();
    UserID = 0;
}

void UAirVRClientConfigImpl::SetConfig(const ONAIRVR_CLIENT_CONFIG& Config, float WorldToMeters)
{
    ClientType = parseAirVRClientType(Config.clientType);
    VideoWidth = Config.videoWidth;
    VideoHeight = Config.videoHeight;
    FrameRate = Config.frameRate;

    float tAngle = FMath::Atan(FMath::Abs(Config.leftEyeCameraNearPlane[1]));
    float bAngle = FMath::Atan(FMath::Abs(Config.leftEyeCameraNearPlane[3]));
    FOV = FMath::RadiansToDegrees<float>(tAngle * FMath::Sign(Config.leftEyeCameraNearPlane[1]) - bAngle * FMath::Sign(Config.leftEyeCameraNearPlane[3]));

    IPD = Config.ipd * WorldToMeters;
    EyeCenterPosition = FVector(Config.eyeCenterPosition.x, Config.eyeCenterPosition.y, Config.eyeCenterPosition.z) * WorldToMeters;
    UserID = Config.userID;
}






