/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Core.h"
#include "AirVRServerFunctionLibrary.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "onairvr_server.h"
#include "Windows/HideWindowsPlatformTypes.h"

class IAirVRTrackingModelContext
{
public:
    virtual void AirVRTrackingModelContextRecenterCameraRigPose() = 0;
};

class FAirVRTrackingModel
{
public:
    static FAirVRTrackingModel* CreateTrackingModel(FAirVRTrackingModelType Type, IAirVRTrackingModelContext* Context);

protected:
    FAirVRTrackingModel(IAirVRTrackingModelContext* InContext);
public:
    virtual ~FAirVRTrackingModel();

public:
    virtual void StartTracking() {}
    virtual void StopTracking();
    virtual void UpdateExternalTrackerLocationAndRotation(const FVector& Location, const FQuat& Rotation) {}

    void UpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config, const ONAIRVR_VECTOR3D& CenterEyePosition, const ONAIRVR_QUATERNION& CenterEyeOrientation);

    FQuat GetHeadOrientation(bool bInHMDSpace) const;
    FVector GetCenterEyePosition() const;
    FVector GetLeftEyePosition() const;
    FVector GetRightEyePosition() const;
    FVector GetLeftEyeOffset() const;
    FVector GetRightEyeOffset() const;
    FMatrix GetHMDToPlayerSpaceMatrix() const;

protected:
    IAirVRTrackingModelContext* GetContext() const  { return Context; }

    virtual FQuat GetHMDTrackingRootRotation() const;
    virtual void OnUpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config, 
                                 const FVector HMDSpaceCenterEyePosition, const FQuat& HMDSpaceCenterEyeOrientation,
                                 FVector& OutPlayerSpaceCenterEyePosition, FQuat& OutPlayerSpaceCenterEyeOrientation) = 0;

private:
    IAirVRTrackingModelContext* Context;
    FVector HMDSpaceCenterEyePosition;
    FQuat HMDSpaceCenterEyeOrientation;
    FVector PlayerSpaceCenterEyePosition;
    FQuat PlayerSpaceCenterEyeOrientation;
    float IPD;
    FMatrix HMDToPlayerSpaceMatrix;
};
