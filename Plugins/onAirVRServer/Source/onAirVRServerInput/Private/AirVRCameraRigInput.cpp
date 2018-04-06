/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRCameraRigInput.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerFunctionLibrary.h"
#include "AirVRServerInput.h"
#include "GenericPlatform/IInputInterface.h"

#include "AllowWindowsPlatformTypes.h"
#include "onairvr_input.h"
#include "HideWindowsPlatformTypes.h"

void FAirVRCameraRigInput::GetControllerOrientationAndPosition(FRotator& OutOrientation, FVector& OutPosition)
{
    FQuat Orientation;
    UAirVRServerFunctionLibrary::GetInputTransform(ControllerID, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::Transform, OutPosition, Orientation);

    OutOrientation = Orientation.Rotator();
}

void FAirVRCameraRigInput::UpdateAndDispatch(FGenericApplicationMessageHandler& MessageHandler)
{
    // Touchpad
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Touchpad, (uint8_t)AirVRTouchpadKey::ExtAxis2DPosition, FAirVRInputKey::TouchpadPositionX.GetFName(), FAirVRInputKey::TouchpadPositionY.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Touchpad, (uint8_t)AirVRTouchpadKey::ExtButtonTouch, FAirVRInputKey::TouchpadTouch.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Touchpad, (uint8_t)AirVRTouchpadKey::ButtonBack, FAirVRInputKey::TouchpadBackButton.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Touchpad, (uint8_t)AirVRTouchpadKey::ButtonUp, FAirVRInputKey::TouchpadUp.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Touchpad, (uint8_t)AirVRTouchpadKey::ButtonDown, FAirVRInputKey::TouchpadDown.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Touchpad, (uint8_t)AirVRTouchpadKey::ButtonLeft, FAirVRInputKey::TouchpadLeft.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Touchpad, (uint8_t)AirVRTouchpadKey::ButtonRight, FAirVRInputKey::TouchpadRight.GetFName());

    // Gamepad
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::Axis2DLThumbstick, EKeys::Gamepad_LeftX.GetFName(), EKeys::Gamepad_LeftY.GetFName());
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::Axis2DRThumbstick, EKeys::Gamepad_RightX.GetFName(), EKeys::Gamepad_RightY.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::AxisLIndexTrigger, EKeys::Gamepad_LeftTriggerAxis.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::AxisRIndexTrigger, EKeys::Gamepad_RightTriggerAxis.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonA, EKeys::Gamepad_FaceButton_Bottom.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonB, EKeys::Gamepad_FaceButton_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonX, EKeys::Gamepad_FaceButton_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonY, EKeys::Gamepad_FaceButton_Top.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonStart, EKeys::Gamepad_Special_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonBack, EKeys::Gamepad_Special_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonLShoulder, EKeys::Gamepad_LeftShoulder.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonRShoulder, EKeys::Gamepad_RightShoulder.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonLThumbstick, EKeys::Gamepad_LeftThumbstick.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonRThumbstick, EKeys::Gamepad_RightThumbstick.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonDpadUp, EKeys::Gamepad_DPad_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonDpadDown, EKeys::Gamepad_DPad_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonDpadLeft, EKeys::Gamepad_DPad_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ButtonDpadRight, EKeys::Gamepad_DPad_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonLIndexTrigger, EKeys::Gamepad_LeftTrigger.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonRIndexTrigger, EKeys::Gamepad_RightTrigger.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonLThumbstickUp, EKeys::Gamepad_LeftStick_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonLThumbstickDown, EKeys::Gamepad_LeftStick_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonLThumbstickLeft, EKeys::Gamepad_LeftStick_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonLThumbstickRight, EKeys::Gamepad_LeftStick_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonRThumbstickUp, EKeys::Gamepad_RightStick_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonRThumbstickDown, EKeys::Gamepad_RightStick_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonRThumbstickLeft, EKeys::Gamepad_RightStick_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Gamepad, (uint8_t)AirVRGamepadKey::ExtButtonRThumbstickRight, EKeys::Gamepad_RightStick_Right.GetFName());

    // Tracked controller
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ExtAxis2DTouchPosition, 
                         EKeys::MotionController_Left_Thumbstick_X.GetFName(), EKeys::MotionController_Left_Thumbstick_Y.GetFName(), EKeys::MotionController_Right_Thumbstick_X.GetFName(), EKeys::MotionController_Right_Thumbstick_Y.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ExtButtonTouch, FAirVRInputKey::TrackedControllerTouchpadTouch.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ButtonTouchpad, EKeys::MotionController_Left_Thumbstick.GetFName(),
                                                                                                                                        EKeys::MotionController_Right_Thumbstick.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ButtonBack, EKeys::MotionController_Left_Shoulder.GetFName(),
                                                                                                                                    EKeys::MotionController_Right_Shoulder.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ButtonIndexTrigger, EKeys::MotionController_Left_Trigger.GetFName(),
                                                                                                                                            EKeys::MotionController_Right_Trigger.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ButtonUp, EKeys::MotionController_Left_Thumbstick_Up.GetFName(),
                                                                                                                                  EKeys::MotionController_Right_Thumbstick_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ButtonDown, EKeys::MotionController_Left_Thumbstick_Down.GetFName(),
                                                                                                                                    EKeys::MotionController_Right_Thumbstick_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ButtonLeft, EKeys::MotionController_Left_Thumbstick_Left.GetFName(),
                                                                                                                                    EKeys::MotionController_Right_Thumbstick_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::TrackedController, (uint8_t)AirVRTrackedControllerKey::ButtonRight, EKeys::MotionController_Left_Thumbstick_Right.GetFName(),
                                                                                                                                     EKeys::MotionController_Right_Thumbstick_Right.GetFName());
}

void FAirVRCameraRigInput::DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName KeyForX, FName KeyForY)
{
    FVector2D Axis2D = UAirVRServerFunctionLibrary::GetInputAxis2D(ControllerID, Device, Control);

    MessageHandler.OnControllerAnalog(KeyForX, ControllerID, Axis2D.X);
    MessageHandler.OnControllerAnalog(KeyForY, ControllerID, Axis2D.Y);
}

void FAirVRCameraRigInput::DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKeyForX, FName LeftKeyForY, FName RightKeyForX, FName RightKeyForY)
{
    FVector2D Axis2D = UAirVRServerFunctionLibrary::GetInputAxis2D(ControllerID, Device, Control);

    MessageHandler.OnControllerAnalog(LeftKeyForX, ControllerID, Axis2D.X);
    MessageHandler.OnControllerAnalog(LeftKeyForY, ControllerID, Axis2D.Y);
    MessageHandler.OnControllerAnalog(RightKeyForX, ControllerID, Axis2D.X);
    MessageHandler.OnControllerAnalog(RightKeyForY, ControllerID, Axis2D.Y);
}

void FAirVRCameraRigInput::DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName Key)
{
    MessageHandler.OnControllerAnalog(Key, ControllerID, UAirVRServerFunctionLibrary::GetInputAxis(ControllerID, Device, Control));
}

void FAirVRCameraRigInput::DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKey, FName RightKey)
{
    float Value = UAirVRServerFunctionLibrary::GetInputAxis(ControllerID, Device, Control);
    MessageHandler.OnControllerAnalog(LeftKey, ControllerID, Value);
    MessageHandler.OnControllerAnalog(RightKey, ControllerID, Value);
}

void FAirVRCameraRigInput::DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName Key)
{
    if (UAirVRServerFunctionLibrary::GetInputButtonDown(ControllerID, Device, Control)) {
        MessageHandler.OnControllerButtonPressed(Key, ControllerID, false);
    }
    if (UAirVRServerFunctionLibrary::GetInputButtonUp(ControllerID, Device, Control)) {
        MessageHandler.OnControllerButtonReleased(Key, ControllerID, false);
    }
}

void FAirVRCameraRigInput::DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, FName LeftKey, FName RightKey)
{
    if (UAirVRServerFunctionLibrary::GetInputButtonDown(ControllerID, Device, Control)) {
        MessageHandler.OnControllerButtonPressed(LeftKey, ControllerID, false);
        MessageHandler.OnControllerButtonPressed(RightKey, ControllerID, false);
    }
    if (UAirVRServerFunctionLibrary::GetInputButtonUp(ControllerID, Device, Control)) {
        MessageHandler.OnControllerButtonReleased(LeftKey, ControllerID, false);
        MessageHandler.OnControllerButtonReleased(RightKey, ControllerID, false);
    }
}
