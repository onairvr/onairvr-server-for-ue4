/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRCameraRig.h"
#include "AirVRServerPrivate.h"

#include "AirVRServerHMD.h"

FAirVRCameraRig::FAirVRCameraRig(class FAirVREventDispatcher* InEventDispatcher)
    : PlayerID(-1), EventDispatcher(InEventDispatcher), LastTrackingTimeStamp(0.0), InputStream(this), bIsActivated(false), bEncodeRequested(false)
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

FMatrix FAirVRCameraRig::GetLeftEyeProjectionMatrix() const
{
    return MakeProjectionMatrix(LeftEyeCameraNearPlane[0] * GNearClippingPlane,
                                LeftEyeCameraNearPlane[1] * GNearClippingPlane,
                                LeftEyeCameraNearPlane[2] * GNearClippingPlane,
                                LeftEyeCameraNearPlane[3] * GNearClippingPlane,
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

        OutIsStereoscopic = true;
    }
    else {
        VideoWidth = ScreenViewport.Width();
        VideoHeight = ScreenViewport.Height();

        float AspectRatio = ScreenViewport.Width() > 0 && ScreenViewport.Height() > 0 ? (float)VideoWidth / VideoHeight : 1.0f;
        LeftEyeCameraNearPlane[0] = 1.0f * AspectRatio;
        LeftEyeCameraNearPlane[1] = 1.0f;
        LeftEyeCameraNearPlane[2] = -1.0f * AspectRatio;
        LeftEyeCameraNearPlane[3] = -1.0f;

        OutShouldEncode = false;
        OutIsStereoscopic = false;
    }
}

void FAirVRCameraRig::BindPlayer(int InPlayerID, const ONAIRVR_CLIENT_CONFIG& Config)
{
    PlayerID = InPlayerID;

    VideoWidth = Config.videoWidth;
    VideoHeight = Config.videoHeight;
    for (int i = 0; i < 4; i++) {
        LeftEyeCameraNearPlane[i] = Config.leftEyeCameraNearPlane[i];
    }

	LastTrackingTimeStamp = 0.0;
}

void FAirVRCameraRig::UnbindPlayer()
{
    PlayerID = -1;
}

void FAirVRCameraRig::Update()
{
    if (IsBound()) {
        InputStream.Update();

        ONAIRVR_CLIENT_CONFIG Config;
        onairvr_GetConfig(PlayerID, &Config);

        ONAIRVR_VECTOR3D Position;
        ONAIRVR_QUATERNION Orientation;
        InputStream.GetTransform(ONAIRVR_INPUT_DEVICE_HEADTRACKER, (uint8)AirVRHeadTrackerKey::Transform, LastTrackingTimeStamp, &Position, &Orientation);

        TrackingModel->UpdateEyePose(Config, Position, Orientation);
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
        onairvr_EnableNetworkTimeWarp(PlayerID, bEnable);
    }
}

void FAirVRCameraRig::AirVRTrackingModelContextRecenterCameraRigPose()
{
    if (IsBound()) {
        onairvr_RecenterPose(PlayerID);
    }
}

void FAirVRCameraRig::AirVREventMediaStreamInitialized(int InPlayerID)
{
    if (PlayerID == InPlayerID) {
        ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
            onairvr_InitStreams_RenderThread,
            int, PlayerID, PlayerID,
            {
                onairvr_InitStreams_RenderThread(PlayerID);
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
            onairvr_ResetStreams_RenderThread,
            int, PlayerID, PlayerID,
            {
                onairvr_ResetStreams_RenderThread(PlayerID);
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
            onairvr_CleanupStreams_RenderThread,
            int, PlayerID, PlayerID,
            {
                onairvr_CleanupStreams_RenderThread(PlayerID);
            }
        );
        FlushRenderingCommands();
    }
}

void FAirVRCameraRig::AirVREventInputStreamRemoteInputDeviceRegistered(int InPlayerID, const FString& DeviceName, uint8 DeviceID)
{
    if (PlayerID == InPlayerID) {
        InputStream.HandleRemoteInputDeviceRegistered(DeviceName, DeviceID);
    }
}

void FAirVRCameraRig::AirVREventInputStreamRemoteInputDeviceUnregistered(int InPlayerID, uint8 DeviceID)
{
    if (PlayerID == InPlayerID) {
        InputStream.HandleRemoteInputDeviceUnregistered(DeviceID);
    }
}

FMatrix FAirVRCameraRig::MakeProjectionMatrix(float Left, float Top, float Right, float Bottom, float Near) const
{ 
    return FMatrix(FPlane(   2.0f * Near / (Left - Right),                            0.0f, 0.0f, 0.0f),
                   FPlane(                           0.0f,    2.0f * Near / (Top - Bottom), 0.0f, 0.0f),
                   FPlane((Left + Right) / (Left - Right), (Top + Bottom) / (Top - Bottom), 0.0f, 1.0f),
                   FPlane(                           0.0f,                            0.0f, Near, 0.0f));
}
