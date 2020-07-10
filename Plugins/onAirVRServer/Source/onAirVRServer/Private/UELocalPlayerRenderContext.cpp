/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "UELocalPlayerRenderContext.h"
#include "AirVRServerPrivate.h"

#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "AirVRServerHMD.h"

FUELocalPlayerRenderContext::FUELocalPlayerRenderContext()
    : CurrentLocalPlayerControllerIndex(-1), CurrentPhase(Phase::GameFrame)
{}

FUELocalPlayerRenderContext::~FUELocalPlayerRenderContext()
{}

int32 FUELocalPlayerRenderContext::GetCurrentPlayerControllerId() const
{
    if (IsCurrentLocalPlayerControllerIndexValid() == false) {
        return INVALID_CONTROLLERID;
    }

    return LocalPlayerControllers[CurrentLocalPlayerControllerIndex];
}

void FUELocalPlayerRenderContext::OnStartGameFrame(FWorldContext& Context)
{
    CurrentPhase = Phase::GameFrame;

    LocalPlayerControllers.Empty();
    GetCurrentLocalPlayerControllers(LocalPlayerControllers);
    CurrentLocalPlayerControllerIndex = LocalPlayerControllers.Num() - 1;
}

void FUELocalPlayerRenderContext::OnApplyHMDRotation(APlayerController* PC, FRotator& ViewRotation)
{
    check(CurrentPhase == Phase::GameFrame);

    // Local player controllers are iterated and called this method per tick. (optionally; might be called or not)
    check(IsCurrentLocalPlayerControllerIndexValid());
    CurrentLocalPlayerControllerIndex = (CurrentLocalPlayerControllerIndex == LocalPlayerControllers.Num() - 1) ? 0 : CurrentLocalPlayerControllerIndex + 1;
    check(IsCurrentLocalPlayerControllerIndexValid());

    // Note that FDefaultXRCamera is going to call IXRTrackingSystem::GetCurrentPose().
}

void FUELocalPlayerRenderContext::OnUpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
    check(CurrentPhase == Phase::GameFrame);

    // Local player controllers are iterated and called this method per tick.
    check(IsCurrentLocalPlayerControllerIndexValid());
    CurrentLocalPlayerControllerIndex = (CurrentLocalPlayerControllerIndex == LocalPlayerControllers.Num() - 1) ? 0 : CurrentLocalPlayerControllerIndex + 1;
    check(IsCurrentLocalPlayerControllerIndexValid());

    // Note that FDefaultXRCamera is going to call IXRTrackingSystem::GetCurrentPose().
}

void FUELocalPlayerRenderContext::OnEndGameFrame(FWorldContext& Context)
{
    check(CurrentPhase == Phase::GameFrame);

    CurrentPhase = Phase::Draw;
    LocalPlayerControllers.Empty();
    CurrentLocalPlayerControllerIndex = 0;
}

void FUELocalPlayerRenderContext::OnSetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo)
{
    if (CurrentPhase == Phase::Draw) {
        check(Player->IsLocalPlayerController());

        // This is the first call per local player on Draw phase.
        // Memorize the controller ID of the player then use it on subsequent calls.

        LocalPlayerControllers.Empty();
        LocalPlayerControllers.Add(Player->GetLocalPlayer()->GetControllerId());
    }
}

void FUELocalPlayerRenderContext::OnAdjustViewRect(enum EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY)
{
    check(CurrentPhase == Phase::Draw);
    check(LocalPlayerControllers.Num() == 1);
    check(CurrentLocalPlayerControllerIndex == 0);

    // Use the controller ID of local player from SetupViewPoint().
}

void FUELocalPlayerRenderContext::OnCalculateStereoCameraOffset(const enum EStereoscopicPass StereoPassType, FRotator& ViewRotation, FVector& ViewLocation)
{
    if (CurrentPhase == Phase::Draw) {
        check(LocalPlayerControllers.Num() == 1);
        check(CurrentLocalPlayerControllerIndex == 0);
    }

    // Use the controller ID of local player from SetupViewPoint().
    // Note that FDefaultXRCamera is going to call IXRTrackingSystem::GetRelativeEyePose().
}

void FUELocalPlayerRenderContext::OnGetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType)
{
    check(CurrentPhase == Phase::Draw);
    check(LocalPlayerControllers.Num() == 1);
    check(CurrentLocalPlayerControllerIndex == 0);

    // Use the controller ID of local player from SetupViewPoint().
}

void FUELocalPlayerRenderContext::OnSetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
    if (IsInEditorViewportRenderingDuringPIE()) {
        return;
    }

    check(CurrentPhase == Phase::Draw);
    check(LocalPlayerControllers.Num() == 1);
    check(CurrentLocalPlayerControllerIndex == 0);

    // Use the controller ID of local player from SetupViewPoint().
    // Note that FDefaultXRCamera is going to call IXRTrackingSystem::GetCurrentPose().
}

void FUELocalPlayerRenderContext::OnBeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
    if (IsInEditorViewportRenderingDuringPIE()) {
        return;
    }

    check(CurrentPhase == Phase::Draw);
    CurrentPhase = Phase::Paint;
    LocalPlayerControllers.Empty();
    CurrentLocalPlayerControllerIndex = -1;
}

void FUELocalPlayerRenderContext::OnStereoDisabled()
{
    CurrentPhase = Phase::GameFrame;
    LocalPlayerControllers.Empty();
    CurrentLocalPlayerControllerIndex = -1;
}

void FUELocalPlayerRenderContext::GetCurrentLocalPlayerControllers(TArray<int32>& Result) const
{
    check(GWorld);

    for (FConstPlayerControllerIterator Iter = GWorld->GetPlayerControllerIterator(); Iter; ++Iter) {
        APlayerController* PlayerController = Iter->Get();
        if (PlayerController->IsLocalPlayerController()) {
            Result.Add(PlayerController->GetLocalPlayer()->GetControllerId());
        }
    }
}

bool FUELocalPlayerRenderContext::IsCurrentLocalPlayerControllerIndexValid() const
{
    return 0 <= CurrentLocalPlayerControllerIndex && CurrentLocalPlayerControllerIndex < LocalPlayerControllers.Num();
}

bool FUELocalPlayerRenderContext::IsInEditorViewportRenderingDuringPIE() const
{
    return GIsEditor && GIsPlayInEditorWorld == false;
}
