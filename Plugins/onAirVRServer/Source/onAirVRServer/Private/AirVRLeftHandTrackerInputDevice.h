/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputDevice.h"

class FAirVRLeftHandTrackerInputDevice : public FAirVRInputDevice {
public:
    FAirVRLeftHandTrackerInputDevice();

public:
    // implements FAirVRInputDevice
    virtual FString Name() const { return ONAIRVR_INPUT_DEVICE_LEFT_HAND_TRACKER; }

    virtual void UpdateExtendedControls() override;
};
