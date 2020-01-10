/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "HeadMountedDisplay.h"
#include "HeadMountedDisplayBase.h"
#include "XRRenderTargetManager.h"
#include "DefaultXRCamera.h"
#include "Slate/SceneViewport.h"
#include "UELocalPlayerRenderContext.h"
#include "AirVREventDispatcher.h"
#include "AirVRPlayerCameraRigMap.h"
#include "AirVRInputStream.h"
#include "AirVRServerScreenRenderer.h"
#include "AirVRRenderCameraRigQueue.h"
#include "AirVRServerFunctionLibrary.h"

class FAirVRServerXRCamera : public FDefaultXRCamera
{
public:
    FAirVRServerXRCamera(const FAutoRegister& AutoRegister, IXRTrackingSystem* InTrackingSystem, FUELocalPlayerRenderContext& InContext);
    virtual ~FAirVRServerXRCamera();

public:
    FSceneViewFamily* GetCurrentViewFamily() const  { return CurrentViewFamily; }

public:
    // overrides FDefaultXRCamera
    virtual void ApplyHMDRotation(APlayerController* PC, FRotator& ViewRotation) override;
    virtual bool UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition) override;
    virtual void CalculateStereoCameraOffset(const enum EStereoscopicPass StereoPassType, FRotator& ViewRotation, FVector& ViewLocation) override;

    // overrides ISceneViewExtension of FDefaultXRCamera
    virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
    virtual void SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo) override;
    virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
    virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;
    virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override;

private:
    FUELocalPlayerRenderContext& LocalPlayerRenderContext;
    FSceneViewFamily* CurrentViewFamily;
};

class FAirVRServerHMD : public FHeadMountedDisplayBase, public FXRRenderTargetManager, public TSharedFromThis<FAirVRServerHMD, ESPMode::ThreadSafe>, public IAirVREventListener
{
public:
    FAirVRServerHMD();
    virtual ~FAirVRServerHMD();

public:
    bool IsPlayerBound(int32 PlayerControllerID) const;
    bool IsPlayerActivated(int32 PlayerControllerID) const;
    bool GetConfig(int32 PlayerControllerID, class UAirVRClientConfig*& Config);
    void GetOrientationAndPosition(int32 PlayerControllerID, FQuat& Orientation, FVector& Position) const;
    void ResetOrientationAndPosition(int32 PlayerControllerID);
    void AdjustBitrate(int32 PlayerControllerID, int32 BitrateInKbps);
    void Disconnect(int32 PlayerControllerID);

    bool IsInputDeviceAvailable(int32 PlayerControllerID, FAirVRInputDeviceType Device) const;
    void GetTrackedDeviceRotationAndPosition(int32 PlayerControllerID, FAirVRInputDeviceType Device, FRotator& Rotation, FVector& Position) const;

    void SetCameraRigTrackingModel(int32 PlayerControllerID, FAirVRTrackingModelType TrackingModelType);
    void UpdateCameraRigExternalTrackerLocationAndRotation(int32 PlayerControllerID, const FVector& Location, const FQuat& Rotation);

    void SendAudioFrame(int32 PlayerControllerID, const float* Data, int SampleCount, int Channels, double TimeStamp);
    void SendAudioFrameToAllPlayers(const float* Data, int SampleCount, int Channels, double TimeStamp);

    void EnableNetworkTimeWarp(int32 PlayerControllerID, bool bEnable);

    bool IsDeviceFeedbackEnabled(int32 PlayerControllerID, FAirVRInputDeviceType Device) const;
    void EnableTrackedDeviceFeedback(int32 PlayerControllerID, FAirVRInputDeviceType Device, FString CookieTextureFile, float DepthScaleMultiplier);
    void DisableDeviceFeedback(int32 PlayerControllerID, FAirVRInputDeviceType Device);
    void FeedbackTrackedDevice(int32 PlayerControllerID, FAirVRInputDeviceType Device, const FVector& RayOrigin, const FVector& HitPosition, const FVector& HitNormal);

    //for onAirVRServerInput
    void GetCurrentPlayers(TArray<int32>& Result);
    void GetInputTransform(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control, FVector& Position, FQuat& Orientation) const;
    FVector2D GetInputAxis2D(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const;
    float GetInputAxis(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const;
    float GetInputButtonRaw(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const;
    bool GetInputButton(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const;
    bool GetInputButtonDown(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const;
    bool GetInputButtonUp(int32 PlayerControllerID, FAirVRInputDeviceType Device, uint8 Control) const;

public:
    // implements IXRTrackingSystem interface
    virtual FName GetSystemName() const override
    {
        static FName DefaultName(TEXT("onAirVRServer"));
        return DefaultName;
    }

    virtual TSharedPtr<IXRCamera, ESPMode::ThreadSafe> GetXRCamera(int32 DeviceId = HMDDeviceId) override   { return XRCamera; }
    virtual IHeadMountedDisplay* GetHMDDevice() override                                                    { return this; }
    virtual TSharedPtr<IStereoRendering, ESPMode::ThreadSafe> GetStereoRenderingDevice() override           { return AsShared(); }

    virtual FString GetVersionString() const override;
    virtual bool OnStartGameFrame(FWorldContext& WorldContext) override;
    virtual bool OnEndGameFrame(FWorldContext& WorldContext) override;
    virtual bool DoesSupportPositionalTracking() const override;
    virtual bool HasValidTrackingPosition() override;
    virtual bool EnumerateTrackedDevices(TArray<int32>& OutDevices, EXRTrackedDeviceType Type = EXRTrackedDeviceType::Any) override;
    virtual bool GetTrackingSensorProperties(int32 DeviceId, FQuat& OutOrientation, FVector& OurPosition, FXRSensorProperties& OutSensorProperties) override;
    virtual bool GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition) override;
    virtual bool GetRelativeEyePose(int32 DeviceId, EStereoscopicPass Eye, FQuat& OutOrientation, FVector& OutPosition) override;
    virtual bool IsTracking(int32 DeviceId) override;
    virtual void ResetOrientationAndPosition(float Yaw = 0.0f) override;
    virtual void ResetOrientation(float Yaw = 0.0f) override;
    virtual void ResetPosition() override;
    virtual void SetBaseRotation(const FRotator& BaseRot) override;
    virtual FRotator GetBaseRotation() const override;
    virtual void SetBaseOrientation(const FQuat& BaseOrient) override;
    virtual FQuat GetBaseOrientation() const override;
    virtual void OnBeginPlay(FWorldContext& WorldContext) override;
    virtual void OnEndPlay(FWorldContext& WorldContext) override;
    virtual void RecordAnalytics() override;
	virtual void OnBeginRendering_GameThread() override;
	virtual void OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily) override;

    // implements IHeadMountedDisplay interface
    virtual bool IsHMDConnected() override;
    virtual bool IsHMDEnabled() const override;
    virtual EHMDWornState::Type GetHMDWornState() override;
    virtual void EnableHMD(bool bEnable = true) override;
    virtual bool GetHMDMonitorInfo(MonitorInfo& OutMonitorInfo) override;
    virtual void GetFieldOfView(float& InOutHFOVInDegrees, float& InOutVFOVInDegrees) const override;
    virtual void SetInterpupillaryDistance(float NewInterpupillaryDistance) override;
    virtual float GetInterpupillaryDistance() const override;
    virtual bool IsChromaAbCorrectionEnabled() const override;
    virtual bool GetHMDDistortionEnabled(EShadingPath ShadingPath) const override;
	virtual void UpdatePostProcessSettings(FPostProcessSettings* Settings) override;

    // implements IStereoRendering interface
    virtual IStereoRenderTargetManager* GetRenderTargetManager() override   { return this; }
    virtual IStereoLayers* GetStereoLayers()                                { return nullptr; }

    virtual bool IsStereoEnabled() const override;
    virtual bool EnableStereo(bool stereo = true) override;
    virtual void AdjustViewRect(enum EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
    virtual FMatrix GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType) const override;
    virtual void RenderTexture_RenderThread(class FRHICommandListImmediate& RHICmdList, class FRHITexture2D* BackBuffer, class FRHITexture2D* SrcTexture, FVector2D WindowSize) const override;
    virtual void GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const override;

    // implements FXRRenderTargetManager interface
    virtual void UpdateViewportRHIBridge(bool bUseSeparateRenderTarget, const class FViewport& Viewport, FRHIViewport* const ViewportRHI) override;
    virtual bool ShouldUseSeparateRenderTarget() const override;
    virtual void CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY) override;
    virtual bool NeedReAllocateViewportRenderTarget(const class FViewport& Viewport) override;
    virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 Flags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;

public:
    // implements IAirVREventListener
    virtual void AirVREventSessionConnected(int PlayerID) override;
    virtual void AirVREventSessionDisconnected(int PlayerID) override;
    virtual void AirVREventPlayerCreated(int PlayerID) override;
    virtual void AirVREventPlayerActivated(int PlayerID) override;
    virtual void AirVREventPlayerDeactivated(int PlayerID) override;
    virtual void AirVREventPlayerDestroyed(int PlayerID) override;
    virtual void AirVREventPlayerShowCopyright(int PlayerID) override;

protected:
    // implements FXRTrackingSystemBase virtual methods
    virtual float GetWorldToMetersScale() const override;

private:
    FString GetServerErrorString(int Result) const;
    bool GetLocalPlayerFromViewTarget(class UGameInstance* GameInstance, const class AActor* ViewTarget, ULocalPlayer*& Result) const;
    void HandleStereoEnabled(FWorldContext& WorldContext, bool bEnabled);
    void StartupAirVRServer(FWorldContext& WorldContext);
    void ShutdownAirVRServer();
    void AddAudioSendToMasterSubmix(FWorldContext& WorldContext);
    bool IsTrackedDevice(FAirVRInputDeviceType Device) const;
    const char* ParseInputDeviceName(FAirVRInputDeviceType Device) const;
    uint8 ParseRaycastResultFeedbackControlID(FAirVRInputDeviceType Device) const;
        
private:
    void* AirVRServerDllHandle;
    TSharedPtr<FAirVRServerXRCamera, ESPMode::ThreadSafe> XRCamera;

    bool bStereoEnabled;
    bool bStereoDesired;
    float GameWorldToMeters;

    FAirVREventDispatcher EventDispatcher;
    FAirVRPlayerCameraRigMap PlayerCameraRigMap;
    FAirVRRenderCameraRigQueue RenderCameraRigQueue;
    FAirVRServerScreenRenderer ScreenRenderer;
    FUELocalPlayerRenderContext LocalPlayerRenderContext;

    FVector2D MaxRenderTargetSize;

    FCriticalSection AudioCritSect;
    class USubmixEffectSubmixAirVRServerAudioSendPreset* MasterAudioSendEffectSubmixPreset;
};