/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRCameraRigInput.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerFunctionLibrary.h"
#include "AirVRServerInput.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "onairvr_input.h"
#include "Windows/HideWindowsPlatformTypes.h"

FAirVRCameraRigInput::FAirVRCameraRigInput(int32 InControllerID)
    : ControllerID(InControllerID),
      LeftControllerPosition(FVector::ZeroVector), LeftControllerOrientation(FQuat::Identity),
      RightControllerPosition(FVector::ZeroVector), RightControllerOrientation(FQuat::Identity),
      bPlayingHapticEffect(false)
{}

bool FAirVRCameraRigInput::IsControllerAvailable(EControllerHand DeviceHand) const
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

void FAirVRCameraRigInput::GetControllerOrientationAndPosition(EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition)
{
    FAirVRInputDeviceType DeviceType;
    if (ParseDeviceType(DeviceHand, DeviceType) == false) {
        OutPosition = FVector::ZeroVector;
        OutOrientation = FRotator::ZeroRotator;
        return;
    }

    FVector Position;
    FQuat Orientation;
    UAirVRServerFunctionLibrary::GetInputPose(ControllerID, 
                                              DeviceType,
                                              (uint8)AirVRHandTrackerControl::Pose,
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

void FAirVRCameraRigInput::SetHapticFeedback(EControllerHand DeviceHand, const FHapticFeedbackValues& Values)
{
    FAirVRInputDeviceType DeviceType;
    if (ParseDeviceType(DeviceHand, DeviceType) == false) {
        return;
    }

    SetVibration(DeviceType, Values.Frequency, Values.Amplitude, true);
}

void FAirVRCameraRigInput::SetForceFeedback(const FForceFeedbackValues& Values) 
{
    if (IsControllerAvailable(EControllerHand::Left)) {
        SetVibration(FAirVRInputDeviceType::LeftHandTracker, Values.LeftSmall, Values.LeftLarge, false);
    }
    if (IsControllerAvailable(EControllerHand::Right)) {
        SetVibration(FAirVRInputDeviceType::RightHandTracker, Values.RightSmall, Values.RightLarge, false);
    }
}

bool FAirVRCameraRigInput::ParseDeviceType(EControllerHand DeviceHand, FAirVRInputDeviceType& Result) const
{
    switch (DeviceHand) {
    case EControllerHand::Left:
        Result = FAirVRInputDeviceType::LeftHandTracker;
        return true;
    case EControllerHand::Right:
        Result = FAirVRInputDeviceType::RightHandTracker;
        return true;
    case EControllerHand::AnyHand:
        Result = UAirVRServerFunctionLibrary::IsInputDeviceAvailable(ControllerID, FAirVRInputDeviceType::RightHandTracker) ?
            FAirVRInputDeviceType::RightHandTracker : FAirVRInputDeviceType::LeftHandTracker;
        return true;
    default:
        return false;
    }
}

bool FAirVRCameraRigInput::ParseDeviceType(FForceFeedbackChannelType Channel, FAirVRInputDeviceType& Result) const 
{
    switch (Channel) {
    case FForceFeedbackChannelType::LEFT_SMALL:
    case FForceFeedbackChannelType::LEFT_LARGE:
        Result = FAirVRInputDeviceType::LeftHandTracker;
        return true;
    case FForceFeedbackChannelType::RIGHT_SMALL:
    case FForceFeedbackChannelType::RIGHT_LARGE:
        Result = FAirVRInputDeviceType::RightHandTracker;
        return true;
    default:
        return false;
    }
}

int FAirVRCameraRigInput::LastVibrationIndex(FAirVRInputDeviceType Device) const
{
    switch (Device) {
    case FAirVRInputDeviceType::LeftHandTracker:
        return 0;
    case FAirVRInputDeviceType::RightHandTracker:
        return 1;
    default:
        check(false);
        return 0;
    }
}

void FAirVRCameraRigInput::SetVibration(FAirVRInputDeviceType Device, float Frequency, float Amplitude, bool bIsHapticEffect) 
{
    if (bIsHapticEffect == false && bPlayingHapticEffect) {
        // must not play force feedback while playing haptic effect
        return;
    }

    FHapticFeedbackValues* LastVibration = &LastVibrationValue[LastVibrationIndex(Device)];
    if ((Frequency <= 0 || Amplitude <= 0) &&
        (LastVibration->Frequency <= 0 || LastVibration->Amplitude <= 0)) {
        return;
    }

    LastVibration->Frequency = Frequency;
    LastVibration->Amplitude = Amplitude;

    if (bIsHapticEffect) {
        bPlayingHapticEffect = Frequency > 0 && Amplitude > 0;
    }

    UAirVRServerFunctionLibrary::PendInputVibration(ControllerID, Device, (uint8)AirVRHandTrackerFeedbackControl::Vibration, Frequency, Amplitude);
}

void FAirVRCameraRigInput::UpdateAndDispatch(FGenericApplicationMessageHandler& MessageHandler)
{
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DLThumbstick,
                          FKeyNames({ FAirVRInputKey::LThumbstickX.GetFName(),
                                      EKeys::MotionController_Left_Thumbstick_X.GetFName() }),
                          FKeyNames({ FAirVRInputKey::LThumbstickY.GetFName(),
                                      EKeys::MotionController_Left_Thumbstick_Y.GetFName() }));
    DispatchControlAxis2D(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DRThumbstick,
                          FKeyNames({ FAirVRInputKey::RThumbstickX.GetFName(),
                                      EKeys::MotionController_Right_Thumbstick_X.GetFName() }),
                          FKeyNames({ FAirVRInputKey::RThumbstickY.GetFName(),
                                      EKeys::MotionController_Right_Thumbstick_Y.GetFName() }));

    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisLIndexTrigger,
                        FKeyNames({ FAirVRInputKey::LTriggerAxis.GetFName(),
                                    EKeys::MotionController_Left_TriggerAxis.GetFName() }));
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisRIndexTrigger,
                        FKeyNames({ FAirVRInputKey::RTriggerAxis.GetFName(),
                                    EKeys::MotionController_Right_TriggerAxis.GetFName() }));
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisLHandTrigger,
                        FKeyNames({ FAirVRInputKey::LGripAxis.GetFName(),
                                    EKeys::MotionController_Left_Grip1Axis.GetFName() }));
    DispatchControlAxis(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisRHandTrigger,
                        FKeyNames({ FAirVRInputKey::RGripAxis.GetFName(),
                                    EKeys::MotionController_Right_Grip1Axis.GetFName() }));

    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisLIndexTrigger,
                          FKeyNames({ FAirVRInputKey::LTrigger.GetFName(),
                                      EKeys::MotionController_Left_Trigger.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisRIndexTrigger,
                          FKeyNames({ FAirVRInputKey::RTrigger.GetFName(),
                                      EKeys::MotionController_Right_Trigger.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisLHandTrigger,
                          FKeyNames({ FAirVRInputKey::LGrip.GetFName(),
                                      EKeys::MotionController_Left_Grip1.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::AxisRHandTrigger,
                          FKeyNames({ FAirVRInputKey::RGrip.GetFName(),
                                      EKeys::MotionController_Right_Grip1.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonA,
                          FKeyNames({ FAirVRInputKey::ButtonA.GetFName(),
                                      EKeys::MotionController_Right_FaceButton1.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonB,
                          FKeyNames({ FAirVRInputKey::ButtonB.GetFName(),
                                      EKeys::MotionController_Right_FaceButton2.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonX,
                          FKeyNames({ FAirVRInputKey::ButtonX.GetFName(),
                                      EKeys::MotionController_Left_FaceButton1.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonY,
                          FKeyNames({ FAirVRInputKey::ButtonY.GetFName(),
                                      EKeys::MotionController_Left_FaceButton2.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonStart,
                          FKeyNames({ FAirVRInputKey::ButtonStart.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonBack,
                          FKeyNames({ FAirVRInputKey::ButtonBack.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonLThumbstick,
                          FKeyNames({ FAirVRInputKey::LThumbstick.GetFName(),
                                      EKeys::MotionController_Left_Thumbstick.GetFName() }));
    DispatchControlButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::ButtonRThumbstick,
                          FKeyNames({ FAirVRInputKey::RThumbstick.GetFName(),
                                      EKeys::MotionController_Right_Thumbstick.GetFName() }));
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DLThumbstick,
                                   FKeyNames({ FAirVRInputKey::LThumbstickUp.GetFName(),
                                               EKeys::MotionController_Left_Thumbstick_Up.GetFName() }),
                                   OCS_INPUT_DIRECTION_UP);
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DLThumbstick,
                                   FKeyNames({ FAirVRInputKey::LThumbstickDown.GetFName(),
                                               EKeys::MotionController_Left_Thumbstick_Down.GetFName() }),
                                   OCS_INPUT_DIRECTION_DOWN);
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DLThumbstick,
                                   FKeyNames({ FAirVRInputKey::LThumbstickLeft.GetFName(),
                                               EKeys::MotionController_Left_Thumbstick_Left.GetFName() }),
                                   OCS_INPUT_DIRECTION_LEFT);
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DLThumbstick,
                                   FKeyNames({ FAirVRInputKey::LThumbstickRight.GetFName(),
                                               EKeys::MotionController_Left_Thumbstick_Right.GetFName() }),
                                   OCS_INPUT_DIRECTION_RIGHT);
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DRThumbstick,
                                   FKeyNames({ FAirVRInputKey::RThumbstickUp.GetFName(),
                                               EKeys::MotionController_Right_Thumbstick_Up.GetFName() }),
                                   OCS_INPUT_DIRECTION_UP);
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DRThumbstick,
                                   FKeyNames({ FAirVRInputKey::RThumbstickDown.GetFName(),
                                               EKeys::MotionController_Right_Thumbstick_Down.GetFName() }),
                                   OCS_INPUT_DIRECTION_DOWN);
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DRThumbstick,
                                   FKeyNames({ FAirVRInputKey::RThumbstickLeft.GetFName(),
                                               EKeys::MotionController_Right_Thumbstick_Left.GetFName() }),
                                   OCS_INPUT_DIRECTION_LEFT);
    DispatchControlDirectionButton(MessageHandler, FAirVRInputDeviceType::Controller, (uint8)AirVRControllerControl::Axis2DRThumbstick,
                                   FKeyNames({ FAirVRInputKey::RThumbstickRight.GetFName(),
                                               EKeys::MotionController_Right_Thumbstick_Right.GetFName() }),
                                   OCS_INPUT_DIRECTION_RIGHT);

    //DispatchScreenTouches(MessageHandler);
}

void FAirVRCameraRigInput::DispatchControlAxis2D(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, 
                                                 const FKeyNames& KeysForX, const FKeyNames& KeysForY)
{
    FVector2D Axis2D = UAirVRServerFunctionLibrary::GetInputAxis2D(ControllerID, Device, Control);

    OnControllerAnalog(MessageHandler, KeysForX, ControllerID, Axis2D.X);
    OnControllerAnalog(MessageHandler, KeysForY, ControllerID, Axis2D.Y);
}

void FAirVRCameraRigInput::DispatchControlAxis(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Keys)
{
    OnControllerAnalog(MessageHandler, Keys, ControllerID, UAirVRServerFunctionLibrary::GetInputAxis(ControllerID, Device, Control));
}

void FAirVRCameraRigInput::DispatchControlButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control, const FKeyNames& Keys)
{
    if (UAirVRServerFunctionLibrary::GetInputActivated(ControllerID, Device, Control)) {
        OnControllerButtonPressed(MessageHandler, Keys, ControllerID, false);
    }
    if (UAirVRServerFunctionLibrary::GetInputDeactivated(ControllerID, Device, Control)) {
        OnControllerButtonReleased(MessageHandler, Keys, ControllerID, false);
    }
}

void FAirVRCameraRigInput::DispatchControlDirectionButton(FGenericApplicationMessageHandler& MessageHandler, FAirVRInputDeviceType Device, uint8_t Control,
                                                          const FKeyNames& Keys, OCS_INPUT_DIRECTION Direction) 
{
    if (UAirVRServerFunctionLibrary::GetInputActivated(ControllerID, Device, Control, Direction)) {
        OnControllerButtonPressed(MessageHandler, Keys, ControllerID, false);
    }
    if (UAirVRServerFunctionLibrary::GetInputDeactivated(ControllerID, Device, Control, Direction)) {
        OnControllerButtonReleased(MessageHandler, Keys, ControllerID, false);
    }
}

void FAirVRCameraRigInput::OnControllerAnalog(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Keys, int32 InControllerID, float Value)
{
    for (auto Key : Keys) {
        MessageHandler.OnControllerAnalog(Key, InControllerID, Value);
    }
}
void FAirVRCameraRigInput::OnControllerButtonPressed(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Keys, int32 InControllerID, bool IsRepeat)
{
    for (auto Key : Keys) {
        MessageHandler.OnControllerButtonPressed(Key, InControllerID, IsRepeat);
    }
}
void FAirVRCameraRigInput::OnControllerButtonReleased(FGenericApplicationMessageHandler& MessageHandler, const FKeyNames& Keys, int32 InControllerID, bool IsRepeat)
{
    for (auto Key : Keys) {
        MessageHandler.OnControllerButtonReleased(Key, InControllerID, IsRepeat);
    }
}

void FAirVRCameraRigInput::DispatchScreenTouches(FGenericApplicationMessageHandler& MessageHandler) 
{
    FVector2D Position;
    uint8 State;

    for (uint8 Control = (uint8)AirVRTouchScreenControl::TouchIndexStart; Control <= (uint8)AirVRTouchScreenControl::TouchIndexEnd; Control++) {        
        if (UAirVRServerFunctionLibrary::GetInputActivated(ControllerID, FAirVRInputDeviceType::TouchScreen, Control)) {
            UAirVRServerFunctionLibrary::GetInputTouch2D(ControllerID, FAirVRInputDeviceType::TouchScreen, Control, Position, State);

            MessageHandler.OnTouchStarted(nullptr, Position, 1.0f, (int32)Control, ControllerID);
        }
        else if (UAirVRServerFunctionLibrary::IsInputActive(ControllerID, FAirVRInputDeviceType::TouchScreen, Control)) {
            UAirVRServerFunctionLibrary::GetInputTouch2D(ControllerID, FAirVRInputDeviceType::TouchScreen, Control, Position, State);

            MessageHandler.OnTouchMoved(Position, 1.0f, (int32)Control, ControllerID);
        }
        else if (UAirVRServerFunctionLibrary::GetInputDeactivated(ControllerID, FAirVRInputDeviceType::TouchScreen, Control)) {
            UAirVRServerFunctionLibrary::GetInputTouch2D(ControllerID, FAirVRInputDeviceType::TouchScreen, Control, Position, State);

            MessageHandler.OnTouchEnded(Position, (int32)Control, ControllerID);
        }
    }
}
