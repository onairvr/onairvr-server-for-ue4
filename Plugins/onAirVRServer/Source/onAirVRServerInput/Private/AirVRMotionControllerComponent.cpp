#include "AirVRMotionControllerComponent.h"
#include "AirVRServerInputPrivate.h"

#include "XRMotionControllerBase.h"

UAirVRMotionControllerComponent::UAirVRMotionControllerComponent() 
    : WidgetInteraction(nullptr)
{
    PrimaryComponentTick.bCanEverTick = true;
    bRenderOnClient = false;
    PointerCookieTextureFile = TEXT("NonAssets/Cookie.png");
    PointerCookieDepthScaleMultiplier = 0.03f;
}

void UAirVRMotionControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) 
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex);
    if (!PlayerController) { return; }

    int32 PlayerControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
    FAirVRInputDeviceType InputDevice = GetInputDeviceType();
    if (InputDevice == FAirVRInputDeviceType::Controller) { return; }

    UpdateChildWidgetInteraction(PlayerControllerID, InputDevice);

    if (!WidgetInteraction) { return; }

    if (!UAirVRServerFunctionLibrary::IsInputDeviceAvailable(PlayerControllerID, InputDevice) ||
        !UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(PlayerControllerID, InputDevice)) { return; }

    UAirVRServerFunctionLibrary::UpdateRenderOnClient(PlayerControllerID, InputDevice, bRenderOnClient);
    UAirVRServerFunctionLibrary::EnableRaycastHit(PlayerControllerID, InputDevice, bRenderOnClient);

    if (bRenderOnClient) {
        const FHitResult& Hit = WidgetInteraction->GetLastHitResult();
        if (Hit.IsValidBlockingHit()) {
            FTransform WorldToLocalTransform = GetAttachParent()->GetComponentTransform().Inverse();
            UAirVRServerFunctionLibrary::UpdateRaycastHitResult(PlayerControllerID,
                                                                InputDevice,
                                                                WorldToLocalTransform.TransformPosition(WidgetInteraction->GetComponentLocation()),
                                                                WorldToLocalTransform.TransformPosition(Hit.Location),
                                                                WorldToLocalTransform.TransformVector(Hit.Normal));
        }
        else {
            UAirVRServerFunctionLibrary::UpdateRaycastHitResult(PlayerControllerID, InputDevice, FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector);
        }
    }
}

FAirVRInputDeviceType UAirVRMotionControllerComponent::GetInputDeviceType() const 
{
    if (MotionSource == FXRMotionControllerBase::LeftHandSourceId) {
        return FAirVRInputDeviceType::LeftHandTracker;
    }
    else if (MotionSource == FXRMotionControllerBase::RightHandSourceId) {
        return FAirVRInputDeviceType::RightHandTracker;
    }
    return FAirVRInputDeviceType::Controller;
}

void UAirVRMotionControllerComponent::UpdateChildWidgetInteraction(int32 PlayerControllerID, FAirVRInputDeviceType InputDevice)
{
    UWidgetInteractionComponent* NextWidget = nullptr;

    TArray<USceneComponent*> Children;
    GetChildrenComponents(false, Children);
    for (auto Child : Children) {
        if (Child->IsA(UWidgetInteractionComponent::StaticClass())) {
            NextWidget = Cast<UWidgetInteractionComponent>(Child);
            break;
        }
    }

    if (WidgetInteraction == NextWidget) { return; }

    if (!WidgetInteraction && NextWidget) {
        if (!UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(PlayerControllerID, InputDevice)) {
            IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
            FString PointerCookieFile = FPaths::Combine(FPaths::ProjectContentDir(), PointerCookieTextureFile);
            if (PlatformFile.FileExists(*PointerCookieFile)) {
                UAirVRServerFunctionLibrary::EnableTrackedDeviceFeedback(PlayerControllerID, InputDevice, PointerCookieFile, PointerCookieDepthScaleMultiplier);
            }
        }
    }
    else if (WidgetInteraction && !NextWidget) {
        if (UAirVRServerFunctionLibrary::IsDeviceFeedbackEnabled(PlayerControllerID, InputDevice)) {
            UAirVRServerFunctionLibrary::DisableDeviceFeedback(PlayerControllerID, InputDevice);
        }
    }

    WidgetInteraction = NextWidget;
}
