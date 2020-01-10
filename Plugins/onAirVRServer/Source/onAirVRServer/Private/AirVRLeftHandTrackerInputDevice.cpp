/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRLeftHandTrackerInputDevice.h"
#include "AirVRServerPrivate.h"

FAirVRLeftHandTrackerInputDevice::FAirVRLeftHandTrackerInputDevice() {
    AddControlTransform((uint8)AirVRLeftHandTrackerKey::Transform);
}

void FAirVRLeftHandTrackerInputDevice::UpdateExtendedControls() {
    // no extended controls
}
