/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRPointerWidgetInteractionComponent.h"
#include "AirVRGazePointer.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class ONAIRVRSERVERINPUT_API UAirVRGazePointer : public UAirVRPointerWidgetInteractionComponent
{
    GENERATED_BODY()

protected:
    // implements UAirVRPointerWidgetInteractionComponent virtual methods
    virtual FAirVRInputDeviceType InputDevice() const override   { return FAirVRInputDeviceType::HeadTracker; }

    virtual float GetPrimaryButtonAnalogState(APlayerController* PlayerController) const override;
};
