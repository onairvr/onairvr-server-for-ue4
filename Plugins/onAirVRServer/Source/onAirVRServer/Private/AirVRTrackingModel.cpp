/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRTrackingModel.h"
#include "AirVRServerPrivate.h"

class FAirVRHeadTrackingModel : public FAirVRTrackingModel
{
    friend FAirVRTrackingModel;

protected:
    FAirVRHeadTrackingModel(IAirVRTrackingModelContext* InContext);

    // implement FAirVRTrackingModel
protected:
    virtual void OnUpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config,
        const FVector HMDSpaceCenterEyePosition, const FQuat& HMDSpaceCenterEyeOrientation,
        FVector& OutPlayerSpaceCenterEyePosition, FQuat& OutPlayerSpaceCenterEyeOrientation) override;
};

class FAirVRIPDOnlyTrackingModel : public FAirVRTrackingModel
{
    friend FAirVRTrackingModel;

protected:
    FAirVRIPDOnlyTrackingModel(IAirVRTrackingModelContext* InContext);

    // implements FAirVRTrackingModel
protected:
    virtual void OnUpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config,
        const FVector HMDSpaceCenterEyePosition, const FQuat& HMDSpaceCenterEyeOrientation,
        FVector& OutPlayerSpaceCenterEyePosition, FQuat& OutPlayerSpaceCenterEyeOrientation) override;
};

class FAirVRExternalTrackingModel : public FAirVRTrackingModel
{
    friend FAirVRTrackingModel;

protected:
    FAirVRExternalTrackingModel(IAirVRTrackingModelContext* InContext);

    // implements FAirVRTrackingModel
public:
    virtual void StartTracking() override;
    virtual void UpdateExternalTrackerLocationAndRotation(const FVector& Location, const FQuat& Rotation) override;

protected:
    virtual FQuat GetHMDTrackingRootRotation() const override;
    virtual void OnUpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config,
        const FVector HMDSpaceCenterEyePosition, const FQuat& HMDSpaceCenterEyeOrientation,
        FVector& OutPlayerSpaceCenterEyePosition, FQuat& OutPlayerSpaceCenterEyeOrientation) override;

private:
    FVector TrackerLocation;
    FQuat TrackerRotation;
    FQuat TrackerRotationOnIdentityHMDOrientation;
};

FAirVRTrackingModel* FAirVRTrackingModel::CreateTrackingModel(FAirVRTrackingModelType Type, IAirVRTrackingModelContext* Context)
{
    switch (Type) {
        case FAirVRTrackingModelType::Head:
            return new FAirVRHeadTrackingModel(Context);
        case FAirVRTrackingModelType::InterpupilaryDistanceOnly:
            return new FAirVRIPDOnlyTrackingModel(Context);
        case FAirVRTrackingModelType::ExternalTracker:
            return new FAirVRExternalTrackingModel(Context);
        default:
            break;
    }
    assert(false);
    return nullptr;
}

FAirVRTrackingModel::FAirVRTrackingModel(IAirVRTrackingModelContext* InContext)
    : Context(InContext), HMDSpaceCenterEyePosition(FVector::ZeroVector), HMDSpaceCenterEyeOrientation(FQuat::Identity),
      PlayerSpaceCenterEyePosition(FVector::ZeroVector), PlayerSpaceCenterEyeOrientation(FQuat::Identity), IPD(0.064f),
      HMDToPlayerSpaceMatrix(FMatrix::Identity)
{}

FAirVRTrackingModel::~FAirVRTrackingModel()
{}

void FAirVRTrackingModel::StopTracking() 
{
    HMDToPlayerSpaceMatrix = FMatrix::Identity;
}

void FAirVRTrackingModel::UpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config, const ONAIRVR_VECTOR3D& CenterEyePosition, const ONAIRVR_QUATERNION& CenterEyeOrientation)
{
    IPD = Config.ipd;

    // exponential smoothing
    HMDSpaceCenterEyePosition = FMath::Lerp(HMDSpaceCenterEyePosition, FVector(CenterEyePosition.x, CenterEyePosition.y, CenterEyePosition.z), 0.85f);
    HMDSpaceCenterEyeOrientation = FQuat(CenterEyeOrientation.x, CenterEyeOrientation.y, CenterEyeOrientation.z, CenterEyeOrientation.w);

    //HMDSpaceCenterEyePosition = FVector(CenterEyePosition.x, CenterEyePosition.y, CenterEyePosition.z);

    OnUpdateEyePose(Config, HMDSpaceCenterEyePosition, HMDSpaceCenterEyeOrientation, PlayerSpaceCenterEyePosition, PlayerSpaceCenterEyeOrientation);

    HMDToPlayerSpaceMatrix = FTransform(GetHMDTrackingRootRotation(), PlayerSpaceCenterEyePosition - HMDSpaceCenterEyePosition, FVector::OneVector).ToMatrixWithScale();
}

FQuat FAirVRTrackingModel::GetHeadOrientation(bool bInHMDSpace) const
{
    return bInHMDSpace ? HMDSpaceCenterEyeOrientation : PlayerSpaceCenterEyeOrientation;
}

FVector FAirVRTrackingModel::GetCenterEyePosition() const
{
    return PlayerSpaceCenterEyePosition;
}

FVector FAirVRTrackingModel::GetLeftEyePosition() const
{
    return PlayerSpaceCenterEyePosition + PlayerSpaceCenterEyeOrientation * GetLeftEyeOffset();
}

FVector FAirVRTrackingModel::GetRightEyePosition() const
{
    return PlayerSpaceCenterEyePosition + PlayerSpaceCenterEyeOrientation * GetRightEyeOffset();
}

FVector FAirVRTrackingModel::GetLeftEyeOffset() const
{
    return -FVector::RightVector * 0.5f * IPD;
}

FVector FAirVRTrackingModel::GetRightEyeOffset() const
{
    return FVector::RightVector * 0.5f * IPD;
}

FMatrix FAirVRTrackingModel::GetHMDToPlayerSpaceMatrix() const
{
    return HMDToPlayerSpaceMatrix;
}

FQuat FAirVRTrackingModel::GetHMDTrackingRootRotation() const
{
    return FQuat::Identity;
}

FAirVRHeadTrackingModel::FAirVRHeadTrackingModel(IAirVRTrackingModelContext* InContext) : FAirVRTrackingModel(InContext)
{}

void FAirVRHeadTrackingModel::OnUpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config,
                                              const FVector HMDSpaceCenterEyePosition, const FQuat& HMDSpaceCenterEyeOrientation,
                                              FVector& OutPlayerSpaceCenterEyePosition, FQuat& OutPlayerSpaceCenterEyeOrientation)
{
    OutPlayerSpaceCenterEyePosition = HMDSpaceCenterEyePosition;
    OutPlayerSpaceCenterEyeOrientation = HMDSpaceCenterEyeOrientation;
}

FAirVRIPDOnlyTrackingModel::FAirVRIPDOnlyTrackingModel(IAirVRTrackingModelContext* InContext)
    : FAirVRTrackingModel(InContext)
{}

void FAirVRIPDOnlyTrackingModel::OnUpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config,
                                                 const FVector HMDSpaceCenterEyePosition, const FQuat& HMDSpaceCenterEyeOrientation,
                                                 FVector& OutPlayerSpaceCenterEyePosition, FQuat& OutPlayerSpaceCenterEyeOrientation)
{
    OutPlayerSpaceCenterEyePosition = FVector::ZeroVector;
    OutPlayerSpaceCenterEyeOrientation = HMDSpaceCenterEyeOrientation;
}

FAirVRExternalTrackingModel::FAirVRExternalTrackingModel(IAirVRTrackingModelContext* InContext)
    : FAirVRTrackingModel(InContext),
      TrackerLocation(FVector::ZeroVector),
      TrackerRotation(FQuat::Identity),
      TrackerRotationOnIdentityHMDOrientation(FQuat::Identity)
{}

void FAirVRExternalTrackingModel::StartTracking()
{
    assert(InContext);
    GetContext()->AirVRTrackingModelContextRecenterCameraRigPose();

    TrackerRotationOnIdentityHMDOrientation = FRotator(0.0f, TrackerRotation.Rotator().Yaw, 0.0f).Quaternion();
}

void FAirVRExternalTrackingModel::UpdateExternalTrackerLocationAndRotation(const FVector& Location, const FQuat& Rotation)
{
    TrackerLocation = Location;
    TrackerRotation = Rotation;
}

FQuat FAirVRExternalTrackingModel::GetHMDTrackingRootRotation() const
{
    return TrackerRotationOnIdentityHMDOrientation;
}

void FAirVRExternalTrackingModel::OnUpdateEyePose(const ONAIRVR_CLIENT_CONFIG& Config,
                                                  const FVector HMDSpaceCenterEyePosition, const FQuat& HMDSpaceCenterEyeOrientation,
                                                  FVector& OutPlayerSpaceCenterEyePosition, FQuat& OutPlayerSpaceCenterEyeOrientation)
{
    OutPlayerSpaceCenterEyePosition = TrackerLocation;
    OutPlayerSpaceCenterEyeOrientation = TrackerRotationOnIdentityHMDOrientation * HMDSpaceCenterEyeOrientation;
}
