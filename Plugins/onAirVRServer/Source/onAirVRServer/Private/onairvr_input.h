/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#define ONAIRVR_INPUT_DEVICE_HEADTRACKER            "HeadTracker"
#define ONAIRVR_INPUT_DEVICE_LEFT_HAND_TRACKER      "LeftHandTracker"
#define ONAIRVR_INPUT_DEVICE_RIGHT_HAND_TRACKER     "RightHandTracker"
#define ONAIRVR_INPUT_DEVICE_CONTROLLER             "Controller"

enum class AirVRHeadTrackerKey
{
    Transform = 0,
    RaycastHitResult,

    // ADD ADDITIONAL KEYS HERE

    Max
};

enum class AirVRLeftHandTrackerKey 
{
    Transform = 0,
    RaycastHitResult,

    // ADD ADDITIONAL KEYS HERE

    Max
};

enum class AirVRRightHandTrackerKey 
{
    Transform = 0,
    RaycastHitResult,

    // ADD ADDITIONAL KEYS HERE

    Max
};

enum class AirVRControllerKey {
    Touchpad = 0,

    ButtonTouchpad,
    ButtonUp,
    ButtonDown,
    ButtonLeft,
    ButtonRight,

    Axis2DLThumbstick,
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
    ButtonLShoulder,
    ButtonRShoulder,

    // ADD ADDITIONAL KEYS HERE

    Axis2DTouchpad,
    TouchTouchpad,
    ButtonLIndexTrigger,
    ButtonRIndexTrigger,
    ButtonLHandTrigger,
    ButtonRHandTrigger,
    ButtonLThumbstickUp,
    ButtonLThumbstickDown,
    ButtonLThumbstickLeft,
    ButtonLThumbstickRight,
    ButtonRThumbstickUp,
    ButtonRThumbstickDown,
    ButtonRThumbstickLeft,
    ButtonRThumbstickRight,

    Max
};
