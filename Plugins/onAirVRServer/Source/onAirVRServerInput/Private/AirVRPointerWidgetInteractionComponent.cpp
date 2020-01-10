/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRPointerWidgetInteractionComponent.h"
#include "AirVRServerInputPrivate.h"

UAirVRPointerWidgetInteractionComponent::UAirVRPointerWidgetInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CookieTextureFile = TEXT("NonAssets/cookie.png");
    DepthScaleMultiplier = 0.03f;
    PrimaryButtonPressed = false;
}

void UAirVRPointerWidgetInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (UAirVRServerFunctionLibrary::IsInputDeviceAvailable(PlayerControllerID, InputDevice()) &&
        UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(PlayerControllerID, InputDevice()) == false) {
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        FString CookieFile = FPaths::Combine(ProjectContentDir(), CookieTextureFile);
        if (PlatformFile.FileExists(*CookieFile)) {
            UAirVRServerFunctionLibrary::EnableTrackedDeviceFeedback(PlayerControllerID, InputDevice(), CookieFile, DepthScaleMultiplier);
        }
    }

    FRotator Rotation;
    FVector Position;
    if (InputDevice() == FAirVRInputDeviceType::HeadTracker) {
        UAirVRServerFunctionLibrary::GetOrientationAndPosition(PlayerControllerID, Rotation, Position);
    }
    else if (InputDevice() == FAirVRInputDeviceType::RightHandTracker) {
        UAirVRServerFunctionLibrary::GetTrackedDeviceOrientationAndPosition(PlayerControllerID, FAirVRInputDeviceType::RightHandTracker, Rotation, Position);
    }
    else {
        check(false);
    }

    // TODO handle in the case a camera compoenent is not used and the actor eyes view point is different to the Actor's root component.  

    SetRelativeLocationAndRotation(Position, Rotation);

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), PlayerControllerID);
    check(PlayerController);
    
    float State = GetPrimaryButtonAnalogState(PlayerController);
    if (PrimaryButtonPressed == false && State > 0.0f) {
        PressPointerKey(EKeys::LeftMouseButton);
        PrimaryButtonPressed = true;
    }
    else if (PrimaryButtonPressed && State == 0.0f) {
        ReleasePointerKey(EKeys::LeftMouseButton);
        PrimaryButtonPressed = false;
    }

    if (UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(PlayerControllerID, InputDevice())) {
        const FHitResult& Hit = GetLastHitResult();
        if (Hit.IsValidBlockingHit()) {
            FTransform WorldToLocalTransform = GetAttachParent()->GetComponentTransform().Inverse();
            UAirVRServerFunctionLibrary::FeedbackTrackedDevice(PlayerControllerID, InputDevice(),
                                                               WorldToLocalTransform.TransformPosition(GetComponentLocation()),
                                                               WorldToLocalTransform.TransformPosition(Hit.Location),
                                                               WorldToLocalTransform.TransformVector(Hit.Normal));
        }
        else {
            UAirVRServerFunctionLibrary::FeedbackTrackedDevice(PlayerControllerID, InputDevice(),
                                                               FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector);;
        }
    }
}

void UAirVRPointerWidgetInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(PlayerControllerID, InputDevice())) {
        UAirVRServerFunctionLibrary::DisableDeviceFeedback(PlayerControllerID, InputDevice());
    }
}

FString UAirVRPointerWidgetInteractionComponent::ProjectContentDir() const
{
#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 18)
    return FPaths::ProjectContentDir();
#else
    return FPaths::GameContentDir();
#endif
}
