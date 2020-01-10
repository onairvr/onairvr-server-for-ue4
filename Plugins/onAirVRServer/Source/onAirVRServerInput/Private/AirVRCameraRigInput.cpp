/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRCameraRigInput.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerFunctionLibrary.h"
#include "AirVRServerInput.h"
#include "GenericPlatform/IInputInterface.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "onairvr_input.h"
#include "Windows/HideWindowsPlatformTypes.h"

bool FAirVRCameraRigInput::IsControllerAvailable(const EControllerHand DeviceHand) const
{
    switch (DeviceHand) {
    case EControllerHand::Left:
        return UAirVRServerFunctionLibrary::IsInputDeviceAvailable(ControllerID, FAirVRInputDeviceType::LeftHandTracker);
    case EControllerHand::Right:
        return UAirVRServerFunctionLibrary::IsInputDeviceAvailable(ControllerID, FAirVRInputDeviceType::RightHandTracker);
    case EControllerHand::AnyHand:
        return UAirVRServerFunctionLibrary::IsInputDeviceAvailable(ControllerID, FAirVRInputDeviceType::LeftHandTracker) ||
            UAirVRServerFunctionLibrary::IsInputDeviceAvailable(ControllerID, FAirVRInputDeviceType::RightHandTracker);
    }
    return false;
}

void FAirVRCameraRigInput::GetControllerOrientationAndPosition(const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition)
{
    FAirVRInputDeviceType DeviceType = FAirVRInputDeviceType::RightHandTracker;
    switch (DeviceHand) {
    case EControllerHand::Left:
        DeviceType = FAirVRInputDeviceType::LeftHandTracker;
        break;
    case EControllerHand::Right:
        break;
    case EControllerHand::AnyHand:
        DeviceType = UAirVRServerFunctionLibrary::IsInputDeviceAvailable(ControllerID, FAirVRInputDeviceType::RightHandTracker) ?
            FAirVRInputDeviceType::RightHandTracker : FAirVRInputDeviceType::LeftHandTracker;
        break;
    default:
        OutPosition = FVector::ZeroVector;
        OutOrientation = FRotator::ZeroRotator;
        return;
    }

    FVector Position;
    FQuat Orientation;
    UAirVRServerFunctionLibrary::GetInputTransform(ControllerID, 
                                                   DeviceType, 
                                                   DeviceType == FAirVRInputDeviceType::LeftHandTracker ? (uint8)AirVRLeftHandTrackerKey::Transform : (uint8)AirVRRightHandTrackerKey::Transform,
                                                   Position, 
                                                   Orientation);

    // exponential smoothing
    switch (DeviceType) {
    case FAirVRInputDeviceType::LeftHandTracker:
        LeftControllerPosition = FMath::Lerp(LeftControllerPosition, Position, 0.85f);
        LeftControllerOrientation = FQuat::Slerp(LeftControllerOrientation, Orientation, 0.85f);

        OutPosition = LeftControllerPosition;
        OutOrientation = LeftControllerOrientation.Rotator();
        break;
    case FAirVRInputDeviceType::RightHandTracker:
        RightControllerPosition = FMath::Lerp(RightControllerPosition, Position, 0.85f);
        RightControllerOrientation = FQuat::Slerp(RightControllerOrientation, Orientation, 0.85f);

        OutPosition = RightControllerPosition;
        OutOrientation = RightControllerOrientation.Rotator();
        break;
    }
}

void FAirVRCameraRigInput::UpdateAndDispatch(FGenericApplicationMessageHandler& MessageHandler)
{
    // Touchpad
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DTouchpad, FAirVRInputKey::TouchpadPositionX.GetFName(), FAirVRInputKey::TouchpadPositionY.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::TouchTouchpad, FAirVRInputKey::TouchpadTouch.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonBack, FAirVRInputKey::TouchpadBackButton.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonUp, FAirVRInputKey::TouchpadUp.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonDown, FAirVRInputKey::TouchpadDown.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLeft, FAirVRInputKey::TouchpadLeft.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRight, FAirVRInputKey::TouchpadRight.GetFName());

    // Gamepad
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DLThumbstick, EKeys::Gamepad_LeftX.GetFName(), EKeys::Gamepad_LeftY.GetFName());
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DRThumbstick, EKeys::Gamepad_RightX.GetFName(), EKeys::Gamepad_RightY.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisLIndexTrigger, EKeys::Gamepad_LeftTriggerAxis.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisRIndexTrigger, EKeys::Gamepad_RightTriggerAxis.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonA, EKeys::Gamepad_FaceButton_Bottom.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonB, EKeys::Gamepad_FaceButton_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonX, EKeys::Gamepad_FaceButton_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonY, EKeys::Gamepad_FaceButton_Top.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonStart, EKeys::Gamepad_Special_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonBack, EKeys::Gamepad_Special_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLShoulder, EKeys::Gamepad_LeftShoulder.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRShoulder, EKeys::Gamepad_RightShoulder.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstick, EKeys::Gamepad_LeftThumbstick.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstick, EKeys::Gamepad_RightThumbstick.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonUp, EKeys::Gamepad_DPad_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonDown, EKeys::Gamepad_DPad_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLeft, EKeys::Gamepad_DPad_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRight, EKeys::Gamepad_DPad_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLIndexTrigger, EKeys::Gamepad_LeftTrigger.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRIndexTrigger, EKeys::Gamepad_RightTrigger.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickUp, EKeys::Gamepad_LeftStick_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickDown, EKeys::Gamepad_LeftStick_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickLeft, EKeys::Gamepad_LeftStick_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickRight, EKeys::Gamepad_LeftStick_Right.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickUp, EKeys::Gamepad_RightStick_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickDown, EKeys::Gamepad_RightStick_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickLeft, EKeys::Gamepad_RightStick_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickRight, EKeys::Gamepad_RightStick_Right.GetFName());

    // Controllers
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DLThumbstick,
                          EKeys::MotionController_Left_Thumbstick_X.GetFName(), EKeys::MotionController_Left_Thumbstick_Y.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisLIndexTrigger, EKeys::MotionController_Left_TriggerAxis.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisLHandTrigger, EKeys::MotionController_Left_Grip1Axis.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstick, EKeys::MotionController_Left_Thumbstick.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLIndexTrigger, EKeys::MotionController_Left_Trigger.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLHandTrigger, EKeys::MotionController_Left_Grip1.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonX, EKeys::MotionController_Left_FaceButton1.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonY, EKeys::MotionController_Left_FaceButton2.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickUp, EKeys::MotionController_Left_Thumbstick_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickDown, EKeys::MotionController_Left_Thumbstick_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickLeft, EKeys::MotionController_Left_Thumbstick_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickRight, EKeys::MotionController_Left_Thumbstick_Right.GetFName());

    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DRThumbstick,
                          EKeys::MotionController_Right_Thumbstick_X.GetFName(), EKeys::MotionController_Right_Thumbstick_Y.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisRIndexTrigger, EKeys::MotionController_Right_TriggerAxis.GetFName());
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisRHandTrigger, EKeys::MotionController_Right_Grip1Axis.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstick, EKeys::MotionController_Right_Thumbstick.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRIndexTrigger, EKeys::MotionController_Right_Trigger.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRHandTrigger, EKeys::MotionController_Right_Grip1.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonA, EKeys::MotionController_Right_FaceButton1.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonB, EKeys::MotionController_Right_FaceButton2.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickUp, EKeys::MotionController_Right_Thumbstick_Up.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickDown, EKeys::MotionController_Right_Thumbstick_Down.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickLeft, EKeys::MotionController_Right_Thumbstick_Left.GetFName());
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickRight, EKeys::MotionController_Right_Thumbstick_Right.GetFName());
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
