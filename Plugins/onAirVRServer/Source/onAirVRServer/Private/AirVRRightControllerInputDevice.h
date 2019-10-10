/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputDevice.h"

class FAirVRRightControllerInputDevice : public FAirVRInputDevice 
{
public:
    FAirVRRightControllerInputDevice();
public:
    // implements FAirVRInputDevice
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_RIGHT_CONTROLLER; }

    virtual void UpdateExtendedControls() override;
};

