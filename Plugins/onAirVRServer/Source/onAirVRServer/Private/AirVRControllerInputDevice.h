/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputDevice.h"

class FAirVRControllerInputDevice : public FAirVRInputDevice {
public:
    FAirVRControllerInputDevice();

public:
    // implements FAirVRInputDevice
    virtual FString Name() const { return ONAIRVR_INPUT_DEVICE_CONTROLLER; }

    virtual void UpdateExtendedControls() override;
};
