#include "AirVRMotionControllerComponent.h"
#include "AirVRServerInputPrivate.h"

#include "XRMotionControllerBase.h"

UAirVRMotionControllerComponent::UAirVRMotionControllerComponent() 
    : WidgetInteraction(nullptr)
{
    PrimaryComponentTick.bCanEverTick = true;
    bRenderOnClient = false;
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

    if (!UAirVRServerFunctionLibrary::IsInputDeviceAvailable(PlayerControllerID, InputDevice)) { return; }

    UAirVRServerFunctionLibrary::UpdateRenderOnClient(PlayerControllerID, InputDevice, bRenderOnClient);

    if (!WidgetInteraction || !bRenderOnClient) { return; }

    const FHitResult& Hit = WidgetInteraction->GetLastHitResult();
    if (Hit.IsValidBlockingHit()) {
        FTransform WorldToLocalTransform = GetOwner()->GetRootComponent()->GetComponentTransform().Inverse();
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

    WidgetInteraction = NextWidget;
}
