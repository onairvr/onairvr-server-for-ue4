/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVREventDispatcher.h"
#include "AirVRTrackingModel.h"
#include "AirVRInputStream.h"

class FAirVRCameraRig : public IAirVRTrackingModelContext, public IAirVREventListener
{
public:
    FAirVRCameraRig(class FAirVREventDispatcher* InEventDispatcher);
    virtual ~FAirVRCameraRig();

public:
    class FAirVRInputStream* GetInputStream()   { return &InputStream; }
    bool IsBound() const                        { return PlayerID >= 0; }
    bool IsActivated() const                    { return IsBound() && bIsActivated; }

    void SetTrackingModel(FAirVRTrackingModelType TrackingModelType);
    void UpdateExternalTrackerLocationAndRotation(const FVector& Location, const FQuat& Rotation);

    FMatrix GetLeftEyeProjectionMatrix() const;
    FMatrix GetRightEyeProjectionMatrix() const;
    
	double GetTrackingTimeStamp() const { return LastTrackingTimeStamp; }
    FQuat GetHeadOrientation(bool bInHMDSpace) const;
    FVector GetCenterEyePosition() const;
    FVector GetLeftEyePosition() const;
    FVector GetRightEyePosition() const;
    FVector GetLeftEyeOffset() const;
    FVector GetRightEyeOffset() const;
    FMatrix GetHMDToPlayerSpaceMatrix() const;

    int GetPlayerID() const                 { return PlayerID; }
    int GetVideoWidth() const               { return VideoWidth; }
    int GetVideoHeight() const              { return VideoHeight; }

    void UpdateViewInfo(const FIntRect& ScreenViewport, bool& OutShouldEncode, bool& OutIsStereoscopic);
    void BindPlayer(int InPlayerID, const ONAIRVR_CLIENT_CONFIG& Config);
    void UnbindPlayer();
    void Update();
    void Reset();

    void EnableNetworkTimeWarp(bool bEnable);

public:
    // implements IAirVRTrackingModelContext
    virtual void AirVRTrackingModelContextRecenterCameraRigPose() override;

    // implements IAirVREventListener
    virtual void AirVREventMediaStreamInitialized(int InPlayerID) override;
    virtual void AirVREventMediaStreamStarted(int InPlayerID) override;
    virtual void AirVREventMediaStreamEncodeVideoFrame(int InPlayerID) override;
    virtual void AirVREventMediaStreamStopped(int InPlayerID) override;
    virtual void AirVREventMediaStreamCleanedUp(int InPlayerID) override;
    virtual void AirVREventInputStreamRemoteInputDeviceRegistered(int InPlayerID, const FString& DeviceName, uint8 DeviceID) override;
    virtual void AirVREventInputStreamRemoteInputDeviceUnregistered(int InPlayerID, uint8 DeviceID) override;

private:
    FMatrix MakeProjectionMatrix(float Left, float Top, float Right, float Bottom, float Near) const;
    
private:
    int PlayerID;
    class FAirVREventDispatcher* EventDispatcher;
    class FAirVRTrackingModel* TrackingModel;
	double LastTrackingTimeStamp;
    FAirVRInputStream InputStream;
    bool bIsActivated;
    bool bEncodeRequested;
    int VideoWidth;
    int VideoHeight;
    FIntPoint ViewportMin;
    float LeftEyeCameraNearPlane[4];
};