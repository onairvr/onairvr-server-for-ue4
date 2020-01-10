/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRHeadTrackerInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRHeadTrackerInputDevice::FAirVRHeadTrackerInputDevice()
{
    AddControlTransform((uint8)AirVRHeadTrackerKey::Transform);
}

void FAirVRHeadTrackerInputDevice::UpdateExtendedControls()
{
    // no extended controls
}
