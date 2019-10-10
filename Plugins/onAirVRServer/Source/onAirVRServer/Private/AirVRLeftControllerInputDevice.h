/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputDevice.h"

class FAirVRLeftControllerInputDevice : public FAirVRInputDevice {
public:
    FAirVRLeftControllerInputDevice();
public:
    // implements FAirVRInputDevice
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_LEFT_CONTROLLER; }

    virtual void UpdateExtendedControls() override;
};
