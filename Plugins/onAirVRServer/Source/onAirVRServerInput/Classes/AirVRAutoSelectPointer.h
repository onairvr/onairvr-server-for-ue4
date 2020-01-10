/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRPointerWidgetInteractionComponent.h"
#include "AirVRAutoSelectPointer.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class ONAIRVRSERVERINPUT_API UAirVRAutoSelectPointer : public UAirVRPointerWidgetInteractionComponent
{
    GENERATED_BODY()

public:
    UAirVRAutoSelectPointer();

protected:
    // implements UAirVRPointerWidgetInteractionComponent virtual methods
    virtual FAirVRInputDeviceType InputDevice() const override   { return CurrentDevice; }

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual float GetPrimaryButtonAnalogState(APlayerController* PlayerController) const override;

private:
    FAirVRInputDeviceType CurrentDevice;
};
