/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRCameraRig.h"
#include "AirVRServerPrivate.h"

#include "AirVRServerHMD.h"

FAirVRCameraRig::FAirVRCameraRig(class FAirVREventDispatcher* InEventDispatcher)
    : PlayerID(-1), 
      EventDispatcher(InEventDispatcher), 
      LastTrackingTimeStamp(0.0), 
      InputStream(this), 
      bIsActivated(false), 
      bEncodeRequested(false),
      bStereoscopic(false)
{
    TrackingModel = FAirVRTrackingModel::CreateTrackingModel(FAirVRTrackingModelType::Head, this);
    EventDispatcher->AddListener(this);
}

FAirVRCameraRig::~FAirVRCameraRig()
{
    EventDispatcher->RemoveListener(this);

    assert(TrackingModel);
    delete TrackingModel;
}

void FAirVRCameraRig::SetTrackingModel(FAirVRTrackingModelType Type)
{
    if (TrackingModel) {
        delete TrackingModel;
    }

    TrackingModel = FAirVRTrackingModel::CreateTrackingModel(Type, this);
}

void FAirVRCameraRig::UpdateExternalTrackerLocationAndRotation(const FVector& Location, const FQuat& Rotation)
{
    assert(TrackingModel);
    TrackingModel->UpdateExternalTrackerLocationAndRotation(Location, Rotation);
}

FMatrix FAirVRCameraRig::GetCameraProjectionMatrix() const 
{
    return GetLeftEyeProjectionMatrix();
}

FMatrix FAirVRCameraRig::GetLeftEyeProjectionMatrix() const
{
    return MakeProjectionMatrix(CameraProjection[0] * GNearClippingPlane,
                                CameraProjection[1] * GNearClippingPlane,
                                CameraProjection[2] * GNearClippingPlane,
                                CameraProjection[3] * GNearClippingPlane,
                                GNearClippingPlane);
}

FMatrix FAirVRCameraRig::GetRightEyeProjectionMatrix() const
{
    FMatrix Result = GetLeftEyeProjectionMatrix();
    Result.M[2][0] = -Result.M[2][0];
    return Result;
}

FQuat FAirVRCameraRig::GetHeadOrientation(bool bInHMDSpace) const
{
    assert(TrackingModel);
    return TrackingModel->GetHeadOrientation(bInHMDSpace);
}

FVector FAirVRCameraRig::GetCenterEyePosition() const
{
    assert(TrackingModel);
    return TrackingModel->GetCenterEyePosition();
}

FVector FAirVRCameraRig::GetLeftEyePosition() const
{
    assert(TrackingModel);
    return TrackingModel->GetLeftEyePosition();
}

FVector FAirVRCameraRig::GetRightEyePosition() const
{
    assert(TrackingModel);
    return TrackingModel->GetRightEyePosition();
}

FVector FAirVRCameraRig::GetLeftEyeOffset() const
{
    assert(TrackingModel);
    return TrackingModel->GetLeftEyeOffset();
}

FVector FAirVRCameraRig::GetRightEyeOffset() const
{
    assert(TrackingModel);
    return TrackingModel->GetRightEyeOffset();
}

FMatrix FAirVRCameraRig::GetHMDToPlayerSpaceMatrix() const
{
    assert(TrackingModel);
    return TrackingModel->GetHMDToPlayerSpaceMatrix();
}

void FAirVRCameraRig::UpdateViewInfo(const FIntRect& ScreenViewport, bool& OutShouldEncode, bool& OutIsStereoscopic)
{
    if (IsBound()) {
        OutShouldEncode = bEncodeRequested;
        bEncodeRequested = false;

        OutIsStereoscopic = bStereoscopic;
    }
    else {
        VideoWidth = ScreenViewport.Width();
        VideoHeight = ScreenViewport.Height();

        float AspectRatio = ScreenViewport.Width() > 0 && ScreenViewport.Height() > 0 ? (float)VideoWidth / VideoHeight : 1.0f;
        CameraProjection[0] = -1.0f * AspectRatio;
        CameraProjection[1] = 1.0f;
        CameraProjection[2] = 1.0f * AspectRatio;
        CameraProjection[3] = -1.0f;

        OutShouldEncode = false;
        OutIsStereoscopic = false;
    }
}

void FAirVRCameraRig::BindPlayer(int InPlayerID, const OCS_CLIENT_CONFIG& Config)
{
    PlayerID = InPlayerID;

    bStereoscopic = Config.clientType == OCS_CLIENT_STEREOSCOPIC;
    VideoWidth = Config.videoWidth;
    VideoHeight = Config.videoHeight;
    for (int i = 0; i < 4; i++) {
        CameraProjection[i] = Config.cameraProjection[i];
    }

	LastTrackingTimeStamp = 0;
}

void FAirVRCameraRig::UnbindPlayer()
{
    PlayerID = -1;
}

void FAirVRCameraRig::Update(FWorldContext& WorldContext)
{
    if (IsBound()) {
        InputStream.Update(WorldContext);

        OCS_CLIENT_CONFIG Config;
        ocs_GetConfig(PlayerID, &Config);

        LastTrackingTimeStamp = ocs_GetInputRecvTimestamp(PlayerID);

        OCS_VECTOR3D Position;
        OCS_QUATERNION Orientation;
        if (InputStream.GetPose(AirVRInputDeviceID::HeadTracker,
                                (uint8)AirVRHeadTrackerControl::Pose,
                                &Position,
                                &Orientation)) {
            TrackingModel->UpdateEyePose(Config, Position, Orientation);
        }
    }
}

void FAirVRCameraRig::Reset()
{
    UnbindPlayer();
    InputStream.Reset();

    bIsActivated = false;
    bEncodeRequested = false;
}

void FAirVRCameraRig::EnableNetworkTimeWarp(bool bEnable)
{
    if (IsBound()) {
        ocs_EnableNetworkTimeWarp(PlayerID, bEnable);
    }
}

void FAirVRCameraRig::AirVRTrackingModelContextRecenterCameraRigPose()
{
    if (IsBound()) {
        ocs_RecenterPose(PlayerID);
    }
}

void FAirVRCameraRig::AirVREventMediaStreamInitialized(int InPlayerID)
{
    if (PlayerID == InPlayerID) {
        int InRenderPlayerID = PlayerID;

        ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
            ocs_InitStreams_RenderThread,
            int, InPlayerID, InPlayerID,
            {
                ocs_InitStreams_RenderThread(InPlayerID);
            }
        );

        InputStream.Init();
    }
}

void FAirVRCameraRig::AirVREventMediaStreamStarted(int InPlayerID)
{
    if (PlayerID == InPlayerID) {
        bIsActivated = true;

        assert(TrackingModel);
        TrackingModel->StartTracking();

        InputStream.Start();
    }
}

void FAirVRCameraRig::AirVREventMediaStreamEncodeVideoFrame(int InPlayerID)
{
    if (PlayerID == InPlayerID) {
        bEncodeRequested = true;
        // FAirVRServerHMD::RenderTexture_RenderThread will enqueue encoding events for all camera rigs.
    }
}

void FAirVRCameraRig::AirVREventMediaStreamStopped(int InPlayerID)
{
    if (PlayerID == InPlayerID) {
        bIsActivated = false;
        bEncodeRequested = false;

        assert(TrackingModel);
        TrackingModel->StopTracking();

        ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
            ocs_ResetStreams_RenderThread,
            int, InPlayerID, InPlayerID,
            {
                ocs_ResetStreams_RenderThread(InPlayerID);
            }
        );
        FlushRenderingCommands();

        InputStream.Stop();
    }
}

void FAirVRCameraRig::AirVREventMediaStreamCleanedUp(int InPlayerID)
{
    if (PlayerID == InPlayerID) {
        InputStream.Cleanup();

        ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
            ocs_CleanupStreams_RenderThread,
            int, InPlayerID, InPlayerID,
            {
                ocs_CleanupStreams_RenderThread(InPlayerID);
            }
        );
        FlushRenderingCommands();
    }
}

void FAirVRCameraRig::AirVREventMediaStreamSetCameraProjection(int InPlayerID, const float* Projection) 
{
    if (PlayerID == InPlayerID) {
        for (int i = 0; i < 4; i++) {
            CameraProjection[i] = Projection[i];
        }
    }
}

FMatrix FAirVRCameraRig::MakeProjectionMatrix(float Left, float Top, float Right, float Bottom, float Near) const
{ 
    return FMatrix(FPlane(   2.0f * Near / (Right - Left),                            0.0f, 0.0f, 0.0f),
                   FPlane(                           0.0f,   -2.0f * Near / (Bottom - Top), 0.0f, 0.0f),
                   FPlane((Left + Right) / (Left - Right), (Bottom + Top) / (Bottom - Top), 0.0f, 1.0f),
                   FPlane(                           0.0f,                            0.0f, Near, 0.0f));
}
