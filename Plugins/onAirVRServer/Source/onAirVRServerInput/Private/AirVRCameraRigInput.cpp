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
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonX, 
                          FKeyNames(FAirVRInputKey::LButtonX.GetFName(), EKeys::MotionController_Left_FaceButton1.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonY,
                          FKeyNames(FAirVRInputKey::LButtonY.GetFName(), EKeys::MotionController_Left_FaceButton2.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonStart, 
                          FKeyNames(FAirVRInputKey::LButtonMenu.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLIndexTrigger,
                          FKeyNames(FAirVRInputKey::LTrigger.GetFName(), EKeys::MotionController_Left_Trigger.GetFName()));
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisLIndexTrigger,
                        FKeyNames(FAirVRInputKey::LTriggerAxis.GetFName(), EKeys::MotionController_Left_TriggerAxis.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLHandTrigger,
                          FKeyNames(FAirVRInputKey::LGrip.GetFName(), EKeys::MotionController_Left_Grip1.GetFName()));
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisLHandTrigger,
                        FKeyNames(FAirVRInputKey::LGripAxis.GetFName(), EKeys::MotionController_Left_Grip1Axis.GetFName()));
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DLThumbstick,
                          FKeyNames(FAirVRInputKey::LThumbstickX.GetFName(), EKeys::MotionController_Left_Thumbstick_X.GetFName()),
                          FKeyNames(FAirVRInputKey::LThumbstickY.GetFName(), EKeys::MotionController_Left_Thumbstick_Y.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstick,
                          FKeyNames(FAirVRInputKey::LThumbstick.GetFName(), EKeys::MotionController_Left_Thumbstick.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickUp,
                          FKeyNames(FAirVRInputKey::LThumbstickUp.GetFName(), EKeys::MotionController_Left_Thumbstick_Up.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickDown,
                          FKeyNames(FAirVRInputKey::LThumbstickDown.GetFName(), EKeys::MotionController_Left_Thumbstick_Down.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickLeft,
                          FKeyNames(FAirVRInputKey::LThumbstickLeft.GetFName(), EKeys::MotionController_Left_Thumbstick_Left.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLThumbstickRight,
                          FKeyNames(FAirVRInputKey::LThumbstickRight.GetFName(), EKeys::MotionController_Left_Thumbstick_Right.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonA,
                          FKeyNames(FAirVRInputKey::RButtonA.GetFName(), EKeys::MotionController_Right_FaceButton1.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonB,
                          FKeyNames(FAirVRInputKey::RButtonB.GetFName(), EKeys::MotionController_Right_FaceButton2.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRIndexTrigger,
                          FKeyNames(FAirVRInputKey::RTrigger.GetFName(), EKeys::MotionController_Right_Trigger.GetFName()));
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisRIndexTrigger,
                        FKeyNames(FAirVRInputKey::RTriggerAxis.GetFName(), EKeys::MotionController_Right_TriggerAxis.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRHandTrigger,
                          FKeyNames(FAirVRInputKey::RGrip.GetFName(), EKeys::MotionController_Right_Grip1.GetFName()));
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::AxisRHandTrigger,
                        FKeyNames(FAirVRInputKey::RGripAxis.GetFName(), EKeys::MotionController_Right_Grip1Axis.GetFName()));
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DRThumbstick,
                          FKeyNames(FAirVRInputKey::RThumbstickX.GetFName(), EKeys::MotionController_Right_Thumbstick_X.GetFName()),
                          FKeyNames(FAirVRInputKey::RThumbstickY.GetFName(), EKeys::MotionController_Right_Thumbstick_Y.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstick,
                          FKeyNames(FAirVRInputKey::RThumbstick.GetFName(), EKeys::MotionController_Right_Thumbstick.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickUp,
                          FKeyNames(FAirVRInputKey::RThumbstickUp.GetFName(), EKeys::MotionController_Right_Thumbstick_Up.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickDown,
                          FKeyNames(FAirVRInputKey::RThumbstickDown.GetFName(), EKeys::MotionController_Right_Thumbstick_Down.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickLeft,
                          FKeyNames(FAirVRInputKey::RThumbstickLeft.GetFName(), EKeys::MotionController_Right_Thumbstick_Left.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRThumbstickRight,
                          FKeyNames(FAirVRInputKey::RThumbstickRight.GetFName(), EKeys::MotionController_Right_Thumbstick_Right.GetFName()));

    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonTouchpad,
                          FKeyNames(FAirVRInputKey::TouchpadClick.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::TouchTouchpad,
                          FKeyNames(FAirVRInputKey::TouchpadTouch.GetFName()));
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::Axis2DTouchpad,
                          FKeyNames(FAirVRInputKey::TouchpadX.GetFName()),
                          FKeyNames(FAirVRInputKey::TouchpadY.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonUp,
                          FKeyNames(FAirVRInputKey::TouchpadUp.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonDown,
                          FKeyNames(FAirVRInputKey::TouchpadDown.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonLeft,
                          FKeyNames(FAirVRInputKey::TouchpadLeft.GetFName()));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8_t)AirVRControllerKey::ButtonRight,
                          FKeyNames(FAirVRInputKey::TouchpadRight.GetFName()));
}

void FAirVRCameraRigInput::DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& KeyForX, const FKeyNames& KeyForY)
{
    FVector2D Axis2D = UAirVRServerFunctionLibrary::GetInputAxis2D(ControllerID, Device, Control);

    OnControllerAnalog(MessageHandler, KeyForX, ControllerID, Axis2D.X);
    OnControllerAnalog(MessageHandler, KeyForY, ControllerID, Axis2D.Y);
}

void FAirVRCameraRigInput::DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, 
                                                 const FKeyNames& LeftKeyForX, const FKeyNames& LeftKeyForY, const FKeyNames& RightKeyForX, const FKeyNames& RightKeyForY)
{
    FVector2D Axis2D = UAirVRServerFunctionLibrary::GetInputAxis2D(ControllerID, Device, Control);

    OnControllerAnalog(MessageHandler, LeftKeyForX, ControllerID, Axis2D.X);
    OnControllerAnalog(MessageHandler, LeftKeyForY, ControllerID, Axis2D.Y);
    OnControllerAnalog(MessageHandler, RightKeyForX, ControllerID, Axis2D.X);
    OnControllerAnalog(MessageHandler, RightKeyForY, ControllerID, Axis2D.Y);
}

void FAirVRCameraRigInput::DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Key)
{
    OnControllerAnalog(MessageHandler, Key, ControllerID, UAirVRServerFunctionLibrary::GetInputAxis(ControllerID, Device, Control));
}

void FAirVRCameraRigInput::DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& LeftKey, const FKeyNames& RightKey)
{
    float Value = UAirVRServerFunctionLibrary::GetInputAxis(ControllerID, Device, Control);
    OnControllerAnalog(MessageHandler, LeftKey, ControllerID, Value);
    OnControllerAnalog(MessageHandler, RightKey, ControllerID, Value);
}

void FAirVRCameraRigInput::DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Key)
{
    if (UAirVRServerFunctionLibrary::GetInputButtonDown(ControllerID, Device, Control)) {
        OnControllerButtonPressed(MessageHandler, Key, ControllerID, false);
    }
    if (UAirVRServerFunctionLibrary::GetInputButtonUp(ControllerID, Device, Control)) {
        OnControllerButtonReleased(MessageHandler, Key, ControllerID, false);
    }
}

void FAirVRCameraRigInput::DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& LeftKey, const FKeyNames& RightKey)
{
    if (UAirVRServerFunctionLibrary::GetInputButtonDown(ControllerID, Device, Control)) {
        OnControllerButtonPressed(MessageHandler, LeftKey, ControllerID, false);
        OnControllerButtonPressed(MessageHandler, RightKey, ControllerID, false);
    }
    if (UAirVRServerFunctionLibrary::GetInputButtonUp(ControllerID, Device, Control)) {
        OnControllerButtonReleased(MessageHandler, LeftKey, ControllerID, false);
        OnControllerButtonReleased(MessageHandler, RightKey, ControllerID, false);
    }
}

void FAirVRCameraRigInput::OnControllerAnalog(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Key, int32 InControllerID, float Value)
{
    MessageHandler.OnControllerAnalog(Key.Main, InControllerID, Value);

    if (!Key.Emulated.IsNone()) {
        MessageHandler.OnControllerAnalog(Key.Emulated, InControllerID, Value);
    }
}
void FAirVRCameraRigInput::OnControllerButtonPressed(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Key, int32 InControllerID, bool IsRepeat)
{
    MessageHandler.OnControllerButtonPressed(Key.Main, InControllerID, IsRepeat);

    if (!Key.Emulated.IsNone()) {
        MessageHandler.OnControllerButtonPressed(Key.Emulated, InControllerID, IsRepeat);
    }
}
void FAirVRCameraRigInput::OnControllerButtonReleased(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Key, int32 InControllerID, bool IsRepeat)
{
    MessageHandler.OnControllerButtonReleased(Key.Main, InControllerID, IsRepeat);

    if (!Key.Emulated.IsNone()) {
        MessageHandler.OnControllerButtonReleased(Key.Emulated, InControllerID, IsRepeat);
    }
}
