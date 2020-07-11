/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerHMD.h"
#include "AirVRServerPrivate.h"

#include "GameFramework/WorldSettings.h"
#include "Engine/LocalPlayer.h"
#include "PostProcess/PostProcessHMD.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "dxgiformat.h"
#include "Interfaces/IPluginManager.h"
#include "IAirVRServerPlugin.h"
#include "AirVRCameraRig.h"
#include "AirVRServerFunctionLibrary.h"
#include "AirVRServerAudioSendEffectSubmix.h"
#include "AudioMixerBlueprintLibrary.h"
#include "AirVRServerSettings.h"
#include "AirVRClientConfigImpl.h"
#include "AudioDevice.h"
#include "AudioMixerDevice.h"
#include "Misc/DefaultValueHelper.h"

#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#endif

#define CAMERA_RIG_MAX_VIDEO_WIDTH  3200
#define CAMERA_RIG_MAX_VIDEO_HEIGHT 1600

class FAirVRServerPlugin : public IAirVRServerPlugin
{
public:
    virtual FString GetModuleKeyName() const override
    {
        return TEXT("onAirVRServer");
    }

    virtual TSharedPtr<class IXRTrackingSystem, ESPMode::ThreadSafe> CreateTrackingSystem() override;
    virtual bool IsHMDConnected() { return true; }
};

IMPLEMENT_MODULE(FAirVRServerPlugin, onAirVRServer);
DEFINE_LOG_CATEGORY(LogonAirVRServer);

TSharedPtr<class IXRTrackingSystem, ESPMode::ThreadSafe> FAirVRServerPlugin::CreateTrackingSystem()
{
    TSharedPtr<FAirVRServerHMD, ESPMode::ThreadSafe> Result(new FAirVRServerHMD());
    return Result;
}

FAirVRServerXRCamera::FAirVRServerXRCamera(const FAutoRegister& AutoRegister, IXRTrackingSystem* InTrackingSystem, FUELocalPlayerRenderContext& InContext)
    : FDefaultXRCamera(AutoRegister, InTrackingSystem, FXRTrackingSystemBase::HMDDeviceId), LocalPlayerRenderContext(InContext), CurrentViewFamily(nullptr)
{}

FAirVRServerXRCamera::~FAirVRServerXRCamera()
{}

void FAirVRServerXRCamera::ApplyHMDRotation(APlayerController* PC, FRotator& ViewRotation)
{
    LocalPlayerRenderContext.OnApplyHMDRotation(PC, ViewRotation);

    FDefaultXRCamera::ApplyHMDRotation(PC, ViewRotation);
}

bool FAirVRServerXRCamera::UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
    LocalPlayerRenderContext.OnUpdatePlayerCamera(CurrentOrientation, CurrentPosition);

    return FDefaultXRCamera::UpdatePlayerCamera(CurrentOrientation, CurrentPosition);
}

void FAirVRServerXRCamera::CalculateStereoCameraOffset(const enum EStereoscopicPass StereoPassType, FRotator& ViewRotation, FVector& ViewLocation)
{
    LocalPlayerRenderContext.OnCalculateStereoCameraOffset(StereoPassType, ViewRotation, ViewLocation);

    FDefaultXRCamera::CalculateStereoCameraOffset(StereoPassType, ViewRotation, ViewLocation);
}

void FAirVRServerXRCamera::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
    LocalPlayerRenderContext.OnSetupView(InViewFamily, InView);

    FDefaultXRCamera::SetupView(InViewFamily, InView);
}

void FAirVRServerXRCamera::SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo)
{
    LocalPlayerRenderContext.OnSetupViewPoint(Player, InViewInfo);

    FDefaultXRCamera::SetupViewPoint(Player, InViewInfo);
}

void FAirVRServerXRCamera::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
    LocalPlayerRenderContext.OnBeginRenderViewFamily(InViewFamily);
    CurrentViewFamily = &InViewFamily;

    FDefaultXRCamera::BeginRenderViewFamily(InViewFamily);
}

void FAirVRServerXRCamera::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
    // FDefaultXRCamera get poses on render thread for lower latency. disable for now.
    // TODO make getting poses thread-safe

    //FDefaultXRCamera::PreRenderView_RenderThread(RHICmdList, InView);
}

void FAirVRServerXRCamera::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily)
{
    // FDefaultXRCamera get poses on render thread for lower latency. disable for now.
    // TODO make getting poses thread-safe

    //FDefaultXRCamera::PreRenderViewFamily_RenderThread(RHICmdList, InViewFamily);
}

FAirVRServerHMD::FAirVRServerHMD()
    : FHeadMountedDisplayBase(nullptr),
      bStereoEnabled(false),
      bStereoDesired(false),
      GameWorldToMeters(100.0f),
      PlayerCameraRigMap(&EventDispatcher),
      MaxRenderTargetSize(CAMERA_RIG_MAX_VIDEO_WIDTH, CAMERA_RIG_MAX_VIDEO_HEIGHT),
      bUserUseFixedFrameRate(false),
      UserFixedFrameRate(0.0f),
      MasterAudioSendEffectSubmixPreset(nullptr)
{
    FString PluginPath = IPluginManager::Get().FindPlugin(TEXT("onAirVRServer"))->GetBaseDir();
    AirVRServerDllHandle = FPlatformProcess::GetDllHandle(*FPaths::Combine(*PluginPath, TEXT("Binaries"), TEXT("Win64"), TEXT("ocs.dll")));
    check(AirVRServerDllHandle);

    EventDispatcher.AddListener(this);
    ocs_InitModule(GDynamicRHI->RHIGetNativeDevice(), DXGI_FORMAT_B8G8R8A8_UNORM);

    XRCamera = FSceneViewExtensions::NewExtension<FAirVRServerXRCamera>(this, LocalPlayerRenderContext);
}

FAirVRServerHMD::~FAirVRServerHMD()
{
    ShutdownAirVRServer();
    EventDispatcher.RemoveListener(this);

    check(AirVRServerDllHandle);
    FPlatformProcess::FreeDllHandle(AirVRServerDllHandle);

    check(XRCamera.IsValid());
    XRCamera.Reset();
}

bool FAirVRServerHMD::IsPlayerBound(int32 PlayerControllerID) const
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->IsBound();
    }
    return false;
}

bool FAirVRServerHMD::IsPlayerActivated(int32 PlayerControllerID) const
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->IsActivated();
    }
    return false;
}

bool FAirVRServerHMD::GetConfig(int32 PlayerControllerID, class UAirVRClientConfig*& Config)
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        if (Item.CameraRig->IsBound()) {
            OCS_CLIENT_CONFIG Conf;
            ocs_GetConfig(Item.CameraRig->GetPlayerID(), &Conf);
            UAirVRClientConfigImpl* ConfigImpl = NewObject<UAirVRClientConfigImpl>();
            ConfigImpl->SetConfig(Conf, GetWorldToMetersScale());

            Config = ConfigImpl;
            return true;
        }
    }
    return false;
}

void FAirVRServerHMD::GetOrientationAndPosition(int32 PlayerControllerID, FQuat& Orientation, FVector& Position) const
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        Orientation = Item.CameraRig->GetHeadOrientation(false);
        Position = Item.CameraRig->GetCenterEyePosition() * GetWorldToMetersScale();
    }
    else {
        Orientation = FQuat::Identity;
        Position = FVector::ZeroVector;
    }
}

void FAirVRServerHMD::ResetOrientationAndPosition(int32 PlayerControllerID)
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        if (Item.CameraRig->IsBound()) {
            ocs_RecenterPose(Item.CameraRig->GetPlayerID());
        }
    }
}

void FAirVRServerHMD::Disconnect(int32 PlayerControllerID)
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) && Item.CameraRig->IsBound()) {
        ocs_Disconnect(Item.CameraRig->GetPlayerID());
    }
}

bool FAirVRServerHMD::IsInputDeviceAvailable(int32 PlayerControllerID, FAirVRInputDeviceType Device) const
{
    check(IsInGameThread());

    switch (Device) {
        case FAirVRInputDeviceType::HeadTracker:
            return true;
        case FAirVRInputDeviceType::LeftHandTracker:
        case FAirVRInputDeviceType::RightHandTracker: {
            FAirVRPlayerCameraRigMap::Item Item;
            if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
                uint8 Status = 0;
                if (Item.CameraRig->GetInputStream()->GetState((AirVRInputDeviceID)Device, (uint8)AirVRHandTrackerControl::Status, &Status)) {
                    return Status != 0;
                }
            }
            return false;
        }
        default:
            return false;
    }
}

void FAirVRServerHMD::GetTrackedDeviceRotationAndPosition(int32 PlayerControllerID, FAirVRInputDeviceType Device, FRotator& Rotation, FVector& Position) const
{
    check(IsInGameThread());

    if (IsTrackedDevice(Device) == false) {
        Rotation = FRotator::ZeroRotator;
        Position = FVector::ZeroVector;
        return;
    }

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        switch (Device) {
            case FAirVRInputDeviceType::HeadTracker: {
                Position = Item.CameraRig->GetCenterEyePosition() * GetWorldToMetersScale();
                Rotation = Item.CameraRig->GetHeadOrientation(false).Rotator();
                return;
            }
            case FAirVRInputDeviceType::LeftHandTracker:
            case FAirVRInputDeviceType::RightHandTracker: {
                OCS_VECTOR3D Pos;
                OCS_QUATERNION Rot;
                Item.CameraRig->GetInputStream()->GetPose((AirVRInputDeviceID)Device, (uint8)AirVRHandTrackerControl::Pose, &Pos, &Rot);

                Position = Item.CameraRig->GetHMDToPlayerSpaceMatrix().TransformPosition(VectorFrom(Pos)) * GetWorldToMetersScale();
                Rotation = Item.CameraRig->GetHMDToPlayerSpaceMatrix().Rotator() + QuatFrom(Rot).Rotator();
                return;
            }
        }
    }

    Rotation = FRotator::ZeroRotator;
    Position = FVector::ZeroVector;
}

void FAirVRServerHMD::SetCameraRigTrackingModel(int32 PlayerControllerID, FAirVRTrackingModelType TrackingModelType)
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        Item.CameraRig->SetTrackingModel(TrackingModelType);
    }
}

void FAirVRServerHMD::UpdateCameraRigExternalTrackerLocationAndRotation(int32 PlayerControllerID, const FVector& Location, const FQuat& Rotation)
{
    check(IsInGameThread());

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        Item.CameraRig->UpdateExternalTrackerLocationAndRotation(Location / GetWorldToMetersScale(), Rotation);
    }
}

void FAirVRServerHMD::SendAudioFrame(int32 PlayerControllerID, const float* Data, int SampleCount, int Channels, double TimeStamp)
{
    FScopeLock Lock(&AudioCritSect);

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) && Item.CameraRig->IsBound()) {
        ocs_EncodeAudioFrame(Item.CameraRig->GetPlayerID(), Data, SampleCount, Channels, TimeStamp);
    }
}

void FAirVRServerHMD::SendAudioFrameToAllPlayers(const float* Data, int SampleCount, int Channels, double TimeStamp)
{
    FScopeLock Lock(&AudioCritSect);

    ocs_EncodeAudioFrameForAllPlayers(Data, SampleCount, Channels, TimeStamp);
}

void FAirVRServerHMD::EnableNetworkTimeWarp(int32 PlayerControllerID, bool bEnable)
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        Item.CameraRig->EnableNetworkTimeWarp(bEnable);
    }
}

void FAirVRServerHMD::UpdateRaycastHitResult(int32 PlayerControllerID, FAirVRInputDeviceType Device, const FVector& RayOrigin, const FVector& HitPosition, const FVector& HitNormal)
{
    check(IsInGameThread());

    switch (Device) {
        case FAirVRInputDeviceType::LeftHandTracker:
        case FAirVRInputDeviceType::RightHandTracker: {
            FAirVRPlayerCameraRigMap::Item Item;
            if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
                FMatrix PlayerToHMDSpaceMatrix = Item.CameraRig->GetHMDToPlayerSpaceMatrix().Inverse();
                FVector RayOriginScaled = PlayerToHMDSpaceMatrix.TransformPosition(RayOrigin / GetWorldToMetersScale());
                FVector HitPositionScaled = PlayerToHMDSpaceMatrix.TransformPosition(HitPosition / GetWorldToMetersScale());
                FVector HitNormalScaled = PlayerToHMDSpaceMatrix.TransformVector(HitNormal / GetWorldToMetersScale());

                Item.CameraRig->GetInputStream()->PendRaycastHit((AirVRInputDeviceID)Device, 
                                                                 (uint8)AirVRHandTrackerFeedbackControl::RaycastHitResult,
                                                                 OCSVector3DFrom(RayOriginScaled), 
                                                                 OCSVector3DFrom(HitPositionScaled), 
                                                                 OCSVector3DFrom(HitNormalScaled));
            }
            break;
        }
        default:
            break;  
    }
}

void FAirVRServerHMD::UpdateRenderOnClient(int32 PlayerControllerID, FAirVRInputDeviceType Device, bool bRenderOnClient) 
{
    check(IsInGameThread());

    switch (Device) {
        case FAirVRInputDeviceType::LeftHandTracker:
        case FAirVRInputDeviceType::RightHandTracker: {
            FAirVRPlayerCameraRigMap::Item Item;
            if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
                Item.CameraRig->GetInputStream()->PendState((AirVRInputDeviceID)Device, (uint8)AirVRHandTrackerFeedbackControl::RenderOnClient, bRenderOnClient ? 1 : 0);
            }
            break;
        }
        default:
            break;
    }
}

void FAirVRServerHMD::GetCurrentPlayers(TArray<int32>& Result)
{
    PlayerCameraRigMap.GetBoundPlayers(Result);
    PlayerCameraRigMap.GetUnboundPlayers(Result);
}

uint8_t FAirVRServerHMD::GetInputState(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    uint8 State = 0;

    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) &&
        Item.CameraRig->GetInputStream()->GetState((AirVRInputDeviceID)Device, Control, &State)) {
        return State;
    }
    return 0;
}

uint8_t FAirVRServerHMD::GetInputByteAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const {
    FAirVRPlayerCameraRigMap::Item Item;
    uint8 Axis = 0;

    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) &&
        Item.CameraRig->GetInputStream()->GetByteAxis((AirVRInputDeviceID)Device, Control, &Axis)) {
        return Axis;
    }
    return 0;
}

float FAirVRServerHMD::GetInputAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    float Axis = 0;

    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) &&
        Item.CameraRig->GetInputStream()->GetAxis((AirVRInputDeviceID)Device, Control, &Axis)) {
        return Axis;
    }
    return 0;
}

FVector2D FAirVRServerHMD::GetInputAxis2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    OCS_VECTOR2D Axis2D;

    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) &&
        Item.CameraRig->GetInputStream()->GetAxis2D((AirVRInputDeviceID)Device, Control, &Axis2D)) {
        return FVector2D(Axis2D.x, Axis2D.y);
    }
    return FVector2D::ZeroVector;
}

void FAirVRServerHMD::GetInputPose(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector& Position, FQuat& Orientation) const
{
    FAirVRPlayerCameraRigMap::Item Item;
    OCS_VECTOR3D Pos;
    OCS_QUATERNION Quat;

    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) == false ||
        Item.CameraRig->GetInputStream()->GetPose((AirVRInputDeviceID)Device, Control, &Pos, &Quat) == false) {
        Position = FVector::ZeroVector;
        Orientation = FQuat::Identity;
        return;
    }

    FMatrix HMDToPlayerSpaceMatrix = Item.CameraRig->GetHMDToPlayerSpaceMatrix();
    Position = HMDToPlayerSpaceMatrix.TransformPosition(VectorFrom(Pos)) * GetWorldToMetersScale();
    Orientation = HMDToPlayerSpaceMatrix.ToQuat() * QuatFrom(Quat);
}

void FAirVRServerHMD::GetInputTouch2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector2D& Position, uint8& State) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    OCS_VECTOR2D Pos;

    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item) == false ||
        Item.CameraRig->GetInputStream()->GetTouch2D((AirVRInputDeviceID)Device, Control, &Pos, &State) == false) {
        Position = FVector2D::ZeroVector;
        State = 0;
        return;
    }

    Position = Vector2DFrom(Pos);
}

bool FAirVRServerHMD::IsInputActive(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->GetInputStream()->IsActive((AirVRInputDeviceID)Device, Control);
    }
    return false;
}

bool FAirVRServerHMD::IsInputActive(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->GetInputStream()->IsActive((AirVRInputDeviceID)Device, Control, Direction);
    }
    return false;
}

bool FAirVRServerHMD::GetInputActivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->GetInputStream()->GetActivated((AirVRInputDeviceID)Device, Control);
    }
    return false;
}

bool FAirVRServerHMD::GetInputActivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->GetInputStream()->GetActivated((AirVRInputDeviceID)Device, Control, Direction);
    }
    return false;
}

bool FAirVRServerHMD::GetInputDeactivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->GetInputStream()->GetDeactivated((AirVRInputDeviceID)Device, Control);
    }
    return false;
}

bool FAirVRServerHMD::GetInputDeactivated(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, OCS_INPUT_DIRECTION Direction) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        return Item.CameraRig->GetInputStream()->GetDeactivated((AirVRInputDeviceID)Device, Control, Direction);
    }
    return false;
}

void FAirVRServerHMD::PendInputVibration(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, float Frequency, float Amplitude) const 
{
    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(PlayerControllerID, Item)) {
        Item.CameraRig->GetInputStream()->PendVibration((AirVRInputDeviceID)Device, Control, Frequency, Amplitude);
    }
}

FString FAirVRServerHMD::GetVersionString() const
{
    return FString::Printf(TEXT("onAirVR Server for UE4 2.0.0"));
}

bool FAirVRServerHMD::OnStartGameFrame(FWorldContext& WorldContext)
{
    check(IsInGameThread());

    if (WorldContext.OwningGameInstance == nullptr) {
        // ignore game frame callbacks in editor
        return false;
    }

    GameWorldToMeters = GWorld->GetWorldSettings()->WorldToMeters;

    if (bStereoEnabled != bStereoDesired) {
        bStereoEnabled = bStereoDesired;
        HandleStereoEnabled(WorldContext, bStereoEnabled);
    }

    if (IsStereoEnabled() && WorldContext.GameViewport) {
        FScopeLock Lock(&AudioCritSect);

        PlayerCameraRigMap.OnStartGameFrame(WorldContext);
        ocs_Update(UGameplayStatics::GetWorldDeltaSeconds(WorldContext.World()));
        EventDispatcher.DispatchMessages();

        PlayerCameraRigMap.UpdateCameraRigs(WorldContext);

        LocalPlayerRenderContext.OnStartGameFrame(WorldContext);
    }

    return false; // no effect whatever the return value is.
}

bool FAirVRServerHMD::OnEndGameFrame(FWorldContext& WorldContext)
{
    check(IsInGameThread());

    if (IsStereoEnabled() && WorldContext.GameViewport) {
        PlayerCameraRigMap.OnEndGameFrame(WorldContext, MaxRenderTargetSize);
        LocalPlayerRenderContext.OnEndGameFrame(WorldContext);
    }
    return false;
}

bool FAirVRServerHMD::DoesSupportPositionalTracking() const
{
    // TODO return true if got ready to provide data from camera rigs (EnumerateTrackedDevices, etc.)
    return false;
}

bool FAirVRServerHMD::HasValidTrackingPosition()
{
    // TODO return true if got ready to provide data from camera rigs (EnumerateTrackedDevices, etc.)
    return false;
}

bool FAirVRServerHMD::EnumerateTrackedDevices(TArray<int32>& OutDevices, EXRTrackedDeviceType Type)
{
    // TODO enumerate all camera rigs as HMDs
    return false;
}

bool FAirVRServerHMD::GetTrackingSensorProperties(int32 DeviceId, FQuat& OutOrientation, FVector& OurPosition, FXRSensorProperties& OutSensorProperties)
{
    // TODO set properties of a camera rig
    return false;
}

bool FAirVRServerHMD::GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition)
{
    if (IsInGameThread() == false) {
        CurrentOrientation = FQuat::Identity;
        CurrentPosition = FVector::ZeroVector;
        return true;
    }

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(LocalPlayerRenderContext.GetCurrentPlayerControllerId(), Item) == false) {
        return false;
    }

    CurrentPosition = Item.CameraRig->GetCenterEyePosition() * GetWorldToMetersScale();
    CurrentOrientation = Item.CameraRig->GetHeadOrientation(false);
    return true;
}

bool FAirVRServerHMD::GetRelativeEyePose(int32 DeviceId, EStereoscopicPass Eye, FQuat& OutOrientation, FVector& OutPosition)
{
    OutOrientation = FQuat::Identity;

    FAirVRPlayerCameraRigMap::Item Item;
    if (IsInGameThread() == false || DeviceId != IXRTrackingSystem::HMDDeviceId || (Eye != eSSP_LEFT_EYE && Eye != eSSP_RIGHT_EYE) ||
        PlayerCameraRigMap.GetItem(LocalPlayerRenderContext.GetCurrentPlayerControllerId(), Item) == false) {
        OutPosition = FVector::ZeroVector;
        return false;
    }

    switch (Eye) {
        case eSSP_LEFT_EYE:
            OutPosition = Item.CameraRig->GetLeftEyeOffset() * GetWorldToMetersScale();
            break;
        case eSSP_RIGHT_EYE:
            OutPosition = Item.CameraRig->GetRightEyeOffset() * GetWorldToMetersScale();
            break;
        default:
            break;
    }
    return true;
}

bool FAirVRServerHMD::IsTracking(int32 DeviceId)
{
    // TODO return the right value according to the player and FDefaultXRCamera context

    // do nothing for now
    return false;
}

void FAirVRServerHMD::ResetOrientationAndPosition(float Yaw)
{
    ResetOrientation(Yaw);
    ResetPosition();
}

void FAirVRServerHMD::ResetOrientation(float Yaw)
{
    // TODO reset orientation of the first player
}

void FAirVRServerHMD::ResetPosition()
{
    // TODO reset position of the first player
}

void FAirVRServerHMD::SetBaseRotation(const FRotator& BaseRot)
{
    // TODO set base rotation of the first player
}

FRotator FAirVRServerHMD::GetBaseRotation() const
{
    // TODO return base rotation of the first player
    return FRotator::ZeroRotator;
}

void FAirVRServerHMD::SetBaseOrientation(const FQuat& BaseOrient)
{
    // TODO set base orientation of the first player
}

FQuat FAirVRServerHMD::GetBaseOrientation() const
{
    // TODO return base orientation of the first player
    return FQuat::Identity;
}

void FAirVRServerHMD::OnBeginPlay(FWorldContext& WorldContext)
{
    // do nothing
}

void FAirVRServerHMD::OnEndPlay(FWorldContext& WorldContext)
{
    check(IsInGameThread());

    EnableStereo(false);

    if (bStereoEnabled != bStereoDesired) {
        bStereoEnabled = bStereoDesired;
        HandleStereoEnabled(WorldContext, bStereoEnabled);
    }
}

void FAirVRServerHMD::RecordAnalytics()
{
    // TODO : provide some config info to FEngineAnlaytics or our own analytics provider.
}

void FAirVRServerHMD::OnBeginRendering_GameThread()
{
	check(IsInGameThread());

	if (IsStereoEnabled() == false) {
		return;
	}

	FSceneViewFamily* ViewFamily = XRCamera.Get()->GetCurrentViewFamily();
	check(ViewFamily->RenderTarget);

	FAirVRRenderCameraRigQueue::Item RenderItem;
	RenderItem.RenderTarget = ViewFamily->RenderTarget->GetRenderTargetTexture().GetReference();

	TArray<FAirVRPlayerCameraRigMap::Item> MapItems;
	PlayerCameraRigMap.GetItemsOfBoundPlayers(MapItems);
	PlayerCameraRigMap.GetItemsOfUnboundPlayers(MapItems);
	for (auto MapItem : MapItems) {
		if (MapItem.ShouldRender()) {
			RenderItem.RenderCameraRigRequests.Add(FAirVRRenderCameraRigQueue::RenderCameraRigRequest(MapItem.CameraRig->GetHeadOrientation(true),
				MapItem.CameraRig->GetTrackingTimeStamp(),
				MapItem.bEncode ? MapItem.GetRenderViewport(eSSP_FULL) : FIntRect(),
                MapItem.GetRenderViewport(eSSP_LEFT_EYE),
				MapItem.ScreenViewport,
				MapItem.CameraRig->IsBound() ? MapItem.CameraRig->GetPlayerID() : -1));
		}
	}

	RenderCameraRigQueue.Enqueue(RenderItem);
}

void FAirVRServerHMD::OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	// do nothing
}

bool FAirVRServerHMD::IsHMDConnected()
{
    // consider as connected always because connection here means hardware-aspect wire connection.
    return true;
}

bool FAirVRServerHMD::IsHMDEnabled() const
{
    // consider as enabled always then provide other interface to check the connectivity of clients.
    return true;
}

EHMDWornState::Type FAirVRServerHMD::GetHMDWornState()
{
    // TODO return the HMD worn state of the first player
    return EHMDWornState::Unknown;
}

void FAirVRServerHMD::EnableHMD(bool bEnable)
{
    // do nothing. see IsHMDEnabled()
    return;
}

bool FAirVRServerHMD::GetHMDMonitorInfo(MonitorInfo& OutMonitorInfo)
{
    return false;
}

void FAirVRServerHMD::GetFieldOfView(float& InOutHFOVInDegrees, float& InOutVFOVInDegrees) const
{
    // TODO return FOV of the first player
    InOutHFOVInDegrees = 100.0f;
    InOutVFOVInDegrees = 100.0f;
}

void FAirVRServerHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
    // not used in engine
}

float FAirVRServerHMD::GetInterpupillaryDistance() const
{
    // TODO return IPD of the first player
    return 0.064f;
}

bool FAirVRServerHMD::IsChromaAbCorrectionEnabled() const
{
    return false;
}

bool FAirVRServerHMD::GetHMDDistortionEnabled(EShadingPath /* ShadingPath */) const
{
    return false;
}

// void FAirVRServerHMD::UpdatePostProcessSettings(FPostProcessSettings* Settings)
// {
// 	// not allow to adjust VR scale on server side
// 	Settings->ScreenPercentage = 100.0f;
// }

bool FAirVRServerHMD::IsStereoEnabled() const
{
    return bStereoEnabled;
}

bool FAirVRServerHMD::EnableStereo(bool stereo)
{
    bStereoDesired = stereo;
    return bStereoEnabled;
}

void FAirVRServerHMD::AdjustViewRect(enum EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
    check(IsInGameThread());
    const_cast<FUELocalPlayerRenderContext*>(&LocalPlayerRenderContext)->OnAdjustViewRect(StereoPass, X, Y, SizeX, SizeY);

    FAirVRPlayerCameraRigMap::Item Item;
    if (PlayerCameraRigMap.GetItem(LocalPlayerRenderContext.GetCurrentPlayerControllerId(), Item) == false) {
        return;
    }

    FIntRect Viewport = Item.GetRenderViewport(StereoPass);
    X = Viewport.Min.X;
    Y = Viewport.Min.Y;
    SizeX = Viewport.Width();
    SizeY = Viewport.Height();
}

FMatrix FAirVRServerHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType) const
{
    check(IsInGameThread());
    const_cast<FUELocalPlayerRenderContext*>(&LocalPlayerRenderContext)->OnGetStereoProjectionMatrix(StereoPassType);

    FAirVRPlayerCameraRigMap::Item Item;
    if ((StereoPassType != eSSP_LEFT_EYE && StereoPassType != eSSP_RIGHT_EYE) ||
        PlayerCameraRigMap.GetItem(LocalPlayerRenderContext.GetCurrentPlayerControllerId(), Item) == false) {
        return FMatrix::Identity;
    }

    if (Item.bIsStereoscopic) {
        return StereoPassType == eSSP_LEFT_EYE ? Item.CameraRig->GetLeftEyeProjectionMatrix() : Item.CameraRig->GetRightEyeProjectionMatrix();
    }
    else {
        return Item.CameraRig->GetCameraProjectionMatrix();
    }
}

void FAirVRServerHMD::RenderTexture_RenderThread(class FRHICommandListImmediate& RHICmdList, class FRHITexture2D* BackBuffer, class FRHITexture2D* SrcTexture, FVector2D WindowSize) const
{
    check(IsInRenderingThread());

    FAirVRRenderCameraRigQueue::Item Item;
    if (const_cast<FAirVRRenderCameraRigQueue&>(RenderCameraRigQueue).Dequeue(SrcTexture, Item)) {
        TArray<FAirVRServerScreenRenderer::PanelRenderInfo> Panels;
        for (auto RenderRequest : Item.RenderCameraRigRequests) {
            if (RenderRequest.ShouldEncode()) {
                ocs_EncodeVideoFrame(RenderRequest.PlayerID,
                                     OCS_VIEWPORT((int)RenderRequest.SrcRectEncode.Min.X, (int)RenderRequest.SrcRectEncode.Min.Y,
                                     (int)RenderRequest.SrcRectEncode.Width(), (int)RenderRequest.SrcRectEncode.Height()),
									 RenderRequest.TrackingTimeStamp,
                                     OCS_QUATERNION(RenderRequest.Orientation.X, RenderRequest.Orientation.Y, RenderRequest.Orientation.Z, RenderRequest.Orientation.W),
                                     SrcTexture->GetNativeResource());
            }
            if (RenderRequest.ScreenViewport.Area() > 0) {
                Panels.Add(FAirVRServerScreenRenderer::PanelRenderInfo(RenderRequest.SrcRectScreen, RenderRequest.ScreenViewport));
            }
        }

        if (Panels.Num() > 0) {
            ScreenRenderer.DrawPanels(RHICmdList, Panels, BackBuffer, SrcTexture);
        }
    }
}

void FAirVRServerHMD::GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
{
    if (Context.View.StereoPass == eSSP_LEFT_EYE) {
        EyeToSrcUVOffsetValue.X = 0.0f;
        EyeToSrcUVOffsetValue.Y = 0.0f;

        EyeToSrcUVScaleValue.X = 0.5f;
        EyeToSrcUVScaleValue.Y = 1.0f;
    }
    else {
        EyeToSrcUVOffsetValue.X = 0.5f;
        EyeToSrcUVOffsetValue.Y = 0.0f;

        EyeToSrcUVScaleValue.X = 0.5f;
        EyeToSrcUVScaleValue.Y = 1.0f;
    }
}

void FAirVRServerHMD::UpdateViewportRHIBridge(bool bUseSeparateRenderTarget, const class FViewport& Viewport, FRHIViewport* const ViewportRHI)
{
    // do nothing
}

bool FAirVRServerHMD::ShouldUseSeparateRenderTarget() const
{
    return true;
}

void FAirVRServerHMD::CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY)
{
    if (IsStereoEnabled() == false) {
        return;
    }

    InOutSizeX = (uint32)MaxRenderTargetSize.X;
    InOutSizeY = (uint32)MaxRenderTargetSize.Y;
}

bool FAirVRServerHMD::NeedReAllocateViewportRenderTarget(const class FViewport& Viewport)
{
    if (IsStereoEnabled() == false) {
        return false;
    }

    return Viewport.GetRenderTargetTextureSizeXY().X < MaxRenderTargetSize.X || Viewport.GetRenderTargetTextureSizeXY().Y < MaxRenderTargetSize.Y;
}

bool FAirVRServerHMD::AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
{
    FRHIResourceCreateInfo CreateInfo;
    RHICreateTargetableShaderResource2D(SizeX, SizeY, PF_B8G8R8A8, NumMips, Flags, TargetableTextureFlags, false, CreateInfo, OutTargetableTexture, OutShaderResourceTexture);
    return true;
}

void FAirVRServerHMD::AirVREventSessionConnected(int PlayerID)
{
    UE_LOG(LogonAirVRServer, Log, TEXT("Session connected : %d"), PlayerID);
}

void FAirVRServerHMD::AirVREventSessionDisconnected(int PlayerID)
{
    UE_LOG(LogonAirVRServer, Log, TEXT("Session disconnected : %d"), PlayerID);
}

void FAirVRServerHMD::AirVREventPlayerCreated(int PlayerID)
{
    FAirVRCameraRig* CameraRig = nullptr;
    OCS_CLIENT_CONFIG Config;
    ocs_GetConfig(PlayerID, &Config);

    TArray<int32> UnboundPlayers;
    PlayerCameraRigMap.GetUnboundPlayers(UnboundPlayers);

    if (UnboundPlayers.Num() > 0) {
        // Bind strategy
        // 1. Bind if the controller ID of an unbound player is same as the user ID of the client.
        // 2. Else, an unbound player of the lowest controller ID is bound.

        int32 UserID = 0;
        FAirVRPlayerCameraRigMap::Item Item;

        if (FDefaultValueHelper::ParseInt(Config.userID, UserID)) {
            for (int32 PlayerControllerID : UnboundPlayers) {
                if (UserID == PlayerControllerID) {
                    PlayerCameraRigMap.GetItem(PlayerControllerID, Item);
                    CameraRig = Item.CameraRig;
                    break;
                }
            }
        }
        if (CameraRig == nullptr) {
            UnboundPlayers.Sort();
            PlayerCameraRigMap.GetItem(UnboundPlayers[0], Item);
            CameraRig = Item.CameraRig;
        }
    }

    if (CameraRig) {
        PlayerCameraRigMap.MarkCameraRigAsBound(CameraRig);
        CameraRig->BindPlayer(PlayerID, Config);
        ocs_AcceptPlayer(PlayerID);

        if (GetDefault<UAirVRServerSettings>()->AdaptiveFrameRate) {
            GEngine->FixedFrameRate = GetAdaptiveFrameRate();
        }
    }
}

void FAirVRServerHMD::AirVREventPlayerActivated(int PlayerID)
{
    // do nothing
}

void FAirVRServerHMD::AirVREventPlayerDeactivated(int PlayerID)
{
    // do nothing
}

void FAirVRServerHMD::AirVREventPlayerDestroyed(int PlayerID)
{
    FAirVRPlayerCameraRigMap::Item Item;
    bool Result = PlayerCameraRigMap.GetItemOfBoundPlayer(PlayerID, Item);
    check(Result);

    Item.CameraRig->UnbindPlayer();
    PlayerCameraRigMap.MarkCameraRigAsUnbound(Item.CameraRig);

    if (GetDefault<UAirVRServerSettings>()->AdaptiveFrameRate) {
        GEngine->FixedFrameRate = GetAdaptiveFrameRate();
    }
}

void FAirVRServerHMD::AirVREventPlayerShowCopyright(int PlayerID)
{
    UE_LOG(LogonAirVRServer, Log, TEXT("(C) 2016-present onAirVR. All right reserved."));
}

float FAirVRServerHMD::GetWorldToMetersScale() const
{
    return GameWorldToMeters;
}

FString FAirVRServerHMD::GetServerErrorString(int Result) const
{
    switch (Result) {
        case OCS_RESULT_NOT_SUPPORT_GPU:
            return TEXT("Graphics device is not supported");
        case OCS_RESULT_LICENSE_NOT_VERIFIED:
            return TEXT("License is not verified yet");
        case OCS_RESULT_LICENSE_NOT_FOUND:
            return TEXT("License file not found");
        case OCS_RESULT_LICENSE_INVALID:
            return TEXT("Invalid license file");
        case OCS_RESULT_LICENSE_EXPIRED:
            return TEXT("License expired");
        default:
            break;
    }

    return FString::Printf(TEXT("Unknown error : %d"), Result);
}

bool FAirVRServerHMD::GetLocalPlayerFromViewTarget(class UGameInstance* GameInstance, const class AActor* ViewTarget, ULocalPlayer*& Result) const
{
    for (auto PlayerIt = GameInstance->GetLocalPlayerIterator(); PlayerIt; ++PlayerIt) {
        if ((*PlayerIt)->PlayerController && (*PlayerIt)->PlayerController->GetViewTarget() == ViewTarget) {
            Result = *PlayerIt;
            return true;
        }
    }
    return false;
}

void FAirVRServerHMD::HandleStereoEnabled(FWorldContext& WorldContext, bool bEnabled)
{
    const UAirVRServerSettings* Settings = GetDefault<UAirVRServerSettings>();

    if (bEnabled) {
        if (Settings->AdaptiveFrameRate) {
            bUserUseFixedFrameRate = GEngine->bUseFixedFrameRate;
            UserFixedFrameRate = GEngine->FixedFrameRate;

            GEngine->bUseFixedFrameRate = true;
            GEngine->FixedFrameRate = Settings->MinFrameRate;
        }

        StartupAirVRServer(WorldContext);

        // TODO enable contextual actions in FAirVRServerXRCamera

        GEngine->bForceDisableFrameRateSmoothing = true;

        if (Settings->DisableAudio == false) {
            AddAudioSendToMasterSubmix(WorldContext);
        }
    }
    else {
        //RemoveAudioSendFromMasterSubmix(WorldContext);

        ShutdownAirVRServer();

        // TODO disable contextual actions in FAirVRServerXRCamera

        GEngine->bForceDisableFrameRateSmoothing = false;

        LocalPlayerRenderContext.OnStereoDisabled();

        if (Settings->AdaptiveFrameRate) {
            GEngine->bUseFixedFrameRate = bUserUseFixedFrameRate;
            GEngine->FixedFrameRate = UserFixedFrameRate;
        }
    }
}

void FAirVRServerHMD::StartupAirVRServer(FWorldContext& WorldContext)
{
    const UAirVRServerSettings* Settings = GetDefault<UAirVRServerSettings>();
    check(Settings);
    const_cast<UAirVRServerSettings*>(Settings)->ParseCommandLineArgs();

	int ret = ocs_SetLicenseFile(TCHAR_TO_UTF8((WorldContext.World()->WorldType == EWorldType::Game ? *FPaths::Combine(*FPaths::RootDir(), *Settings->LicenseFilePath) :
                                                                                                      *FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins"), TEXT("onAirVRServer"), TEXT("Resources"), TEXT("onairvr.license")))));
    if (ret == OCS_RESULT_OK) {
        ocs_SetVideoEncoderParameters(120.0f, 0);

        FAudioDevice* AudioDevice = GEngine->GetActiveAudioDevice();
        ret = ocs_StartUp(1, 
                          Settings->PortSTAP,
                          Settings->PortAMP,
                          Settings->LoopbackOnlyForSTAP, 
                          AudioDevice ? (int)AudioDevice->GetSampleRate() : 48000);

        if (ret == OCS_RESULT_OK) {
            ENQUEUE_RENDER_COMMAND(ocs_StartUp_RenderThread)(
                [](FRHICommandListImmediate& RHICmdList)
                {
                    ocs_StartUp_RenderThread(GDynamicRHI->RHIGetNativeDevice());
                }
            );
            FlushRenderingCommands();

            UE_LOG(LogonAirVRServer, Log, TEXT("onAirVR server starts up on port %d"), Settings->PortSTAP);
        }
        else {
            UE_LOG(LogonAirVRServer, Log, TEXT("Failed to start up onAirVR server on port %d"), Settings->PortSTAP);
        }
    }
    else {
        UE_LOG(LogonAirVRServer, Error, TEXT("Invalid or missing license file : %s"), *Settings->LicenseFilePath);
    }
}

void FAirVRServerHMD::ShutdownAirVRServer()
{
    UE_LOG(LogonAirVRServer, Log, TEXT("onAirVR server shutdown."));

    {
        FScopeLock Lock(&AudioCritSect);

        PlayerCameraRigMap.Reset();

        ENQUEUE_RENDER_COMMAND(onairvr_Shutdown_RenderThread)(
            [](FRHICommandListImmediate& RHICmdList)
            {
                ocs_Shutdown_RenderThread();
            }
        );
        
        FlushRenderingCommands();

        ocs_Shutdown();
    }
}

void FAirVRServerHMD::AddAudioSendToMasterSubmix(FWorldContext& WorldContext)
{
    if (WorldContext.World() == nullptr) {
        return;
    }

    // workaround : UAudioMixerBlueprintLibrary seems to check if the audio device is of MixerDevice, but it fails to do.
    FAudioDevice* AudioDevice = WorldContext.World()->GetAudioDevice();
    if (AudioDevice->IsAudioMixerEnabled()) {
        if (MasterAudioSendEffectSubmixPreset == nullptr) {
            MasterAudioSendEffectSubmixPreset = NewObject<USubmixEffectSubmixAirVRServerAudioSendPreset>(GetTransientPackage(), TEXT("Master onAirVR Server Audio Send Submix Effect"));
            MasterAudioSendEffectSubmixPreset->AddToRoot();
        }

        UAudioMixerBlueprintLibrary::AddMasterSubmixEffect(WorldContext.World(), MasterAudioSendEffectSubmixPreset);

        UE_LOG(LogonAirVRServer, Log, TEXT("add audio send submix effect to master submix for broadcasting"));
    }
}

void FAirVRServerHMD::RemoveAudioSendFromMasterSubmix(FWorldContext& WorldContext)
{
    if (WorldContext.World() == nullptr || MasterAudioSendEffectSubmixPreset == nullptr) {
        return;
    }

    FAudioDevice* AudioDevice = WorldContext.World()->GetAudioDevice();
    if (AudioDevice->IsAudioMixerEnabled()) {
        UAudioMixerBlueprintLibrary::RemoveMasterSubmixEffect(WorldContext.World(), MasterAudioSendEffectSubmixPreset);
    }
}

bool FAirVRServerHMD::IsTrackedDevice(FAirVRInputDeviceType Device) const
{
    return Device == FAirVRInputDeviceType::HeadTracker || 
           Device == FAirVRInputDeviceType::LeftHandTracker ||
           Device == FAirVRInputDeviceType::RightHandTracker;
}

float FAirVRServerHMD::GetAdaptiveFrameRate() const 
{
    TArray<FAirVRPlayerCameraRigMap::Item> Items;
    PlayerCameraRigMap.GetItemsOfBoundPlayers(Items);

    float Result = GetDefault<UAirVRServerSettings>()->MinFrameRate;
    for (auto Item : Items) {
        OCS_CLIENT_CONFIG Config;
        if (ocs_GetConfig(Item.CameraRig->GetPlayerID(), &Config)) {
            Result = FMath::Max<float>(Result, Config.frameRate);
        }
    }
    return Result;
}
