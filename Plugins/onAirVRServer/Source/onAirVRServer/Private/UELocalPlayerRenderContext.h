/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "SceneView.h"

class FUELocalPlayerRenderContext
{
public:
    FUELocalPlayerRenderContext();
    ~FUELocalPlayerRenderContext();

public:
    int32 GetCurrentPlayerControllerId() const;

    void OnStartGameFrame(FWorldContext& Context);
    void OnApplyHMDRotation(APlayerController* PC, FRotator& ViewRotation);
    void OnUpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition);
    void OnEndGameFrame(FWorldContext& Context);
    void OnSetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo);
    void OnAdjustViewRect(enum EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY);
    void OnCalculateStereoCameraOffset(const enum EStereoscopicPass StereoPassType, FRotator& ViewRotation, FVector& ViewLocation);
    void OnGetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType);
    void OnSetupView(FSceneViewFamily& InViewFamily, FSceneView& InView);
    void OnBeginRenderViewFamily(FSceneViewFamily& InViewFamily);
    void OnStereoDisabled();

private:
    void GetCurrentLocalPlayerControllers(TArray<int32>& Result) const;
    bool IsCurrentLocalPlayerControllerIndexValid() const;
    bool IsInEditorViewportRenderingDuringPIE() const;

private:
    enum class Phase : uint8
    {
        GameFrame,
        Draw,
        Paint
    };

private:
    TArray<int32> LocalPlayerControllers;
    int CurrentLocalPlayerControllerIndex;
    Phase CurrentPhase;
};
