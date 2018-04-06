/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#define ONAIRVR_INPUT_DEVICE_HEADTRACKER            "HeadTracker"
#define ONAIRVR_INPUT_DEVICE_TOUCHPAD               "Touchpad"
#define ONAIRVR_INPUT_DEVICE_GAMEPAD                "Gamepad"
#define ONAIRVR_INPUT_DEVICE_TRACKED_CONTROLLER     "TrackedController"

enum class AirVRHeadTrackerKey
{
    Transform = 0,
    RaycastHitResult,

    // ADD ADDITIONAL KEYS HERE

    Max
};

enum class AirVRTouchpadKey
{
    Touchpad = 0,

    ButtonBack,
    ButtonUp,
    ButtonDown,
    ButtonLeft,
    ButtonRight,

    // ADD ADDITIONAL KEYS HERE

    ExtAxis2DPosition,
    ExtButtonTouch,

    Max
};

enum class AirVRGamepadKey
{
    Axis2DLThumbstick = 0,
    Axis2DRThumbstick,
    AxisLIndexTrigger,
    AxisRIndexTrigger,

    ButtonA,
    ButtonB,
    ButtonX,
    ButtonY,
    ButtonStart,
    ButtonBack,
    ButtonLShoulder,
    ButtonRShoulder,
    ButtonLThumbstick,
    ButtonRThumbstick,
    ButtonDpadUp,
    ButtonDpadDown,
    ButtonDpadLeft,
    ButtonDpadRight,

    // ADD ADDITIONAL KEYS HERE

    ExtButtonLIndexTrigger,
    ExtButtonRIndexTrigger,
    ExtButtonLThumbstickUp,
    ExtButtonLThumbstickDown,
    ExtButtonLThumbstickLeft,
    ExtButtonLThumbstickRight,
    ExtButtonRThumbstickUp,
    ExtButtonRThumbstickDown,
    ExtButtonRThumbstickLeft,
    ExtButtonRThumbstickRight,

    Max
};

enum class AirVRTrackedControllerKey
{
    Touchpad = 0,
    Transform,
    RaycastHitResult,

    ButtonTouchpad,
    ButtonBack,
    ButtonIndexTrigger,
    ButtonUp,
    ButtonDown,
    ButtonLeft,
    ButtonRight,

    // ADD ADDITIONAL KEYS HERE

    ExtAxis2DTouchPosition,
    ExtButtonTouch,

    Max
};
