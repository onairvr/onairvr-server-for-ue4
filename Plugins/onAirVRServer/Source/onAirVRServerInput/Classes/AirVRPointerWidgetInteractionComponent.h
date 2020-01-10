/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Components/WidgetInteractionComponent.h"
#include "AirVRServerFunctionLibrary.h"
#include "AirVRPointerWidgetInteractionComponent.generated.h"

UCLASS(abstract)
class ONAIRVRSERVERINPUT_API UAirVRPointerWidgetInteractionComponent : public UWidgetInteractionComponent
{
    GENERATED_BODY()

public:
    UAirVRPointerWidgetInteractionComponent();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pointer")
    int32 PlayerControllerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pointer")
    FString CookieTextureFile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pointer")
    float DepthScaleMultiplier;

public:
    // overrides UWidgetInteractionComponent virtual methods
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    virtual FAirVRInputDeviceType InputDevice() const { check(0 && "not implemented"); return FAirVRInputDeviceType::HeadTracker; }
    virtual void GetPointerOrientationAndPosition(int32 PlayerControllerID, FRotator& Rotation, FVector& Position) { check(0 && "not implemented"); }
    virtual float GetPrimaryButtonAnalogState(APlayerController* PlayerController) const { check(0 && "not implemented"); return false; }

private:
    FString ProjectContentDir() const;

private:
    bool PrimaryButtonPressed;
};
