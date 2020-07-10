#pragma once

#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "AirVRServerFunctionLibrary.h"
#include "AirVRMotionControllerComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class ONAIRVRSERVERINPUT_API UAirVRMotionControllerComponent : public UMotionControllerComponent 
{
    GENERATED_BODY()

public:
    UAirVRMotionControllerComponent();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionController")
    bool bRenderOnClient;

public:
    // overrides USceneComponent
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    FAirVRInputDeviceType GetInputDeviceType() const;
    void UpdateChildWidgetInteraction(int32 PlayerControllerID, FAirVRInputDeviceType InputDevice);

private:
    UWidgetInteractionComponent* WidgetInteraction;
};
