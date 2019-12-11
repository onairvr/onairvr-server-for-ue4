/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRRightHandTrackerInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRRightHandTrackerInputDevice::FAirVRRightHandTrackerInputDevice() {
    AddControlTransform((uint8)AirVRRightHandTrackerKey::Transform);
}

void FAirVRRightHandTrackerInputDevice::UpdateExtendedControls() {
    // no extended controls
}
