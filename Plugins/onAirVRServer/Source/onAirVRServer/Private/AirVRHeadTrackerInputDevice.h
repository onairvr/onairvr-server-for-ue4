/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRInputDevice.h"

class FAirVRHeadTrackerInputDevice : public FAirVRInputDevice
{
public:
    FAirVRHeadTrackerInputDevice();

public:
    // implements FAirVRInputDevice
    virtual FString Name() const { return ONAIRVR_INPUT_DEVICE_HEADTRACKER; }

    virtual void UpdateExtendedControls() override;
};
