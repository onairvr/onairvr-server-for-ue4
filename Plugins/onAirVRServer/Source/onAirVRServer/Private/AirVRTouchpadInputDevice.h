/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputDevice.h"

class FAirVRTouchpadInputDevice : public FAirVRInputDevice
{
public:
    FAirVRTouchpadInputDevice();

public:
    // implements FAirVRInputDevice
    virtual FString Name() const override { return ONAIRVR_INPUT_DEVICE_TOUCHPAD; }

    virtual void UpdateExtendedControls() override;
};
