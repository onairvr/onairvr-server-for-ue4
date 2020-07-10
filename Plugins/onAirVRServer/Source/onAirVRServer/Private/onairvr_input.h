/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include <cstdint>

enum class AirVRInputDeviceID : uint8_t
{
    HeadTracker         = 0,
    LeftHandTracker     = 1,
    RightHandTracker    = 2,
    Controller          = 3,
    TouchScreen         = 4
};

enum class AirVRHeadTrackerControl : uint8_t
{
    Pose = 0,

    Max
};

enum class AirVRHandTrackerControl : uint8_t
{
    Status = 0,
    Pose,

    Max
};

enum class AirVRHandTrackerFeedbackControl : uint8_t 
{
    RenderOnClient = 0,
    RaycastHitResult,
    Vibration,

    Max
};

enum class AirVRControllerControl : uint8_t 
{
    Axis2DLThumbstick = 0,
    Axis2DRThumbstick,
    AxisLIndexTrigger,
    AxisRIndexTrigger,
    AxisLHandTrigger,
    AxisRHandTrigger,
    ButtonA,
    ButtonB,
    ButtonX,
    ButtonY,
    ButtonStart,
    ButtonBack,
    ButtonLThumbstick,
    ButtonRThumbstick,

    Max
};

enum class AirVRTouchScreenControl : uint8_t 
{
    TouchIndexStart = 0,
    TouchIndexEnd = 9
};
