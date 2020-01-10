/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputDevice.h"

class FAirVRRightHandTrackerInputDevice : public FAirVRInputDevice {
public:
    FAirVRRightHandTrackerInputDevice();

public:
    // implements FAirVRInputDevice
    virtual FString Name() const { return ONAIRVR_INPUT_DEVICE_RIGHT_HAND_TRACKER; }

    virtual void UpdateExtendedControls() override;
};
