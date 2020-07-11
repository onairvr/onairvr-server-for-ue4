/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerInput.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerFunctionLibrary.h"
#include "AirVRCameraRigInput.h"
#include "IInputDevice.h"
#include "IHapticDevice.h"
#include "Internationalization/internationalization.h"
#include "IAirVRServerPlugin.h"
#include "XRMotionControllerBase.h"
#include "GenericPlatform/IInputInterface.h"

#define LOCTEXT_NAMESPACE "onAirVRServer"

DEFINE_LOG_CATEGORY(LogonAirVRServerInput);

const FKey FAirVRInputKey::LThumbstickX("AirVR_Left_Thumbstick_X");
const FKey FAirVRInputKey::LThumbstickY("AirVR_Left_Thumbstick_Y");
const FKey FAirVRInputKey::RThumbstickX("AirVR_Right_Thumbstick_X");
const FKey FAirVRInputKey::RThumbstickY("AirVR_Right_Thumbstick_Y");
const FKey FAirVRInputKey::TouchpadX("AirVR_Touchpad_X");
const FKey FAirVRInputKey::TouchpadY("AirVR_Touchpad_Y");

const FKey FAirVRInputKey::LTriggerAxis("AirVR_Left_Trigger_Axis");
const FKey FAirVRInputKey::RTriggerAxis("AirVR_Right_Trigger_Axis");
const FKey FAirVRInputKey::LGripAxis("AirVR_Left_Grip_Axis");
const FKey FAirVRInputKey::RGripAxis("AirVR_Right_Grip_Axis");

const FKey FAirVRInputKey::LTrigger("AirVR_Left_Trigger");
const FKey FAirVRInputKey::RTrigger("AirVR_Right_Trigger");
const FKey FAirVRInputKey::LGrip("AirVR_Left_Grip");
const FKey FAirVRInputKey::RGrip("AirVR_Right_Grip");
const FKey FAirVRInputKey::ButtonA("AirVR_Button_A");
const FKey FAirVRInputKey::ButtonB("AirVR_Button_B");
const FKey FAirVRInputKey::ButtonX("AirVR_Button_X");
const FKey FAirVRInputKey::ButtonY("AirVR_Button_Y");
const FKey FAirVRInputKey::ButtonStart("AirVR_Button_Start");
const FKey FAirVRInputKey::ButtonBack("AirVR_Button_Back");
const FKey FAirVRInputKey::LThumbstick("AirVR_Left_Thumbstick");
const FKey FAirVRInputKey::RThumbstick("AirVR_Right_Thumbstick");
const FKey FAirVRInputKey::LThumbstickUp("AirVR_Left_Thumbstick_Up");
const FKey FAirVRInputKey::LThumbstickDown("AirVR_Left_Thumbstick_Down");
const FKey FAirVRInputKey::LThumbstickLeft("AirVR_Left_Thumbstick_Left");
const FKey FAirVRInputKey::LThumbstickRight("AirVR_Left_Thumbstick_Right");
const FKey FAirVRInputKey::RThumbstickUp("AirVR_Right_Thumbstick_Up");
const FKey FAirVRInputKey::RThumbstickDown("AirVR_Right_Thumbstick_Down");
const FKey FAirVRInputKey::RThumbstickLeft("AirVR_Right_Thumbstick_Left");
const FKey FAirVRInputKey::RThumbstickRight("AirVR_Right_Thumbstick_Right");

class FAirVRServerInput : public IInputDevice, public FXRMotionControllerBase, public IHapticDevice {
public:
    FAirVRServerInput(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
    virtual ~FAirVRServerInput();

public:
    // implements IInputDevice pure virtual methods
    virtual void Tick(float DeltaTime) override;
    virtual void SendControllerEvents() override;
    virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
    virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar);
    virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
    virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override;
    virtual IHapticDevice* GetHapticDevice() override { return (IHapticDevice*)this; }
    virtual bool IsGamepadAttached() const override { return true; }

    // implements IMotionController pure virtual methods
    virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMeterScale) const override;
    virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override;

    // implements IHapticDevice
    virtual void SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) override;
    virtual void GetHapticFrequencyRange(float& MinFrequency, float &MaxFrequency) const override;
    virtual float GetHapticAmplitudeScale() const override;

private:
    TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
    TMap<int32, FAirVRCameraRigInput*> PlayerInputMap;

#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 17)
public:
    virtual FName GetMotionControllerDeviceTypeName() const override;
#endif
};

FAirVRServerInput::FAirVRServerInput(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
    : MessageHandler(InMessageHandler)
{
    EKeys::AddMenuCategoryDisplayInfo("onAirVR", LOCTEXT("onAirVRSubCategory", "onAirVR"), TEXT("GraphEditor.PadEvent_16x"));

    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LThumbstickX, LOCTEXT("AirVR_Left_Thumbstick_X", "onAirVR Controller (L) Thumbstick X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LThumbstickY, LOCTEXT("AirVR_Left_Thumbstick_Y", "onAirVR Controller (L) Thumbstick Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RThumbstickX, LOCTEXT("AirVR_Right_Thumbstick_X", "onAirVR Controller (R) Thumbstick X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RThumbstickY, LOCTEXT("AirVR_Right_Thumbstick_Y", "onAirVR Controller (R) Thumbstick Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadX, LOCTEXT("AirVR_Touchpad_X", "onAirVR Touchpad X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadY, LOCTEXT("AirVR_Touchpad_Y", "onAirVR Touchpad Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));

    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LTriggerAxis, LOCTEXT("AirVR_Left_Trigger_Axis", "onAirVR Controller (L) Trigger Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RTriggerAxis, LOCTEXT("AirVR_Right_Trigger_Axis", "onAirVR Controller (R) Trigger Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LGripAxis, LOCTEXT("AirVR_Left_Grip_Axis", "onAirVR Controller (L) Grip Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RGripAxis, LOCTEXT("AirVR_Right_Grip_Axis", "onAirVR Controller (R) Grip Axis"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "onAirVR"));

    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LTrigger, LOCTEXT("AirVR_Left_Trigger", "onAirVR Controller (L) Trigger"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RTrigger, LOCTEXT("AirVR_Right_Trigger", "onAirVR Controller (R) Trigger"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LGrip, LOCTEXT("AirVR_Left_Grip", "onAirVR Controller (L) Grip"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RGrip, LOCTEXT("AirVR_Right_Grip", "onAirVR Controller (R) Grip"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::ButtonA, LOCTEXT("AirVR_Right_Button_A", "onAirVR Controller Button A"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::ButtonB, LOCTEXT("AirVR_Right_Button_Y", "onAirVR Controller Button B"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::ButtonX, LOCTEXT("AirVR_Left_Button_X", "onAirVR Controller Button X"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::ButtonY, LOCTEXT("AirVR_Left_Button_Y", "onAirVR Controller Button Y"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::ButtonStart, LOCTEXT("AirVR_Button_Start", "onAirVR Controller Button Start"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::ButtonBack, LOCTEXT("AirVR_Button_Back", "onAirVR Controller Button Back"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LThumbstick, LOCTEXT("AirVR_Left_Thumbstick", "onAirVR Controller (L) Thumbstick"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RThumbstick, LOCTEXT("AirVR_Right_Thumbstick", "onAirVR Controller (R) Thumbstick"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LThumbstickUp, LOCTEXT("AirVR_Left_Thumbstick_Up", "onAirVR Controller (L) Thumbstick Up"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LThumbstickDown, LOCTEXT("AirVR_Left_Thumbstick_Down", "onAirVR Controller (L) Thumbstick Down"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LThumbstickLeft, LOCTEXT("AirVR_Left_Thumbstick_Left", "onAirVR Controller (L) Thumbstick Left"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::LThumbstickRight, LOCTEXT("AirVR_Left_Thumbstick_Right", "onAirVR Controller (L) Thumbstick Right"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RThumbstickUp, LOCTEXT("AirVR_Right_Thumbstick_Up", "onAirVR Controller (R) Thumbstick Up"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RThumbstickDown, LOCTEXT("AirVR_Right_Thumbstick_Down", "onAirVR Controller (R) Thumbstick Down"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RThumbstickLeft, LOCTEXT("AirVR_Right_Thumbstick_Left", "onAirVR Controller (R) Thumbstick Left"), FKeyDetails::GamepadKey, "onAirVR"));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::RThumbstickRight, LOCTEXT("AirVR_Right_Thumbstick_Right", "onAirVR Controller (R) Thumbstick Right"), FKeyDetails::GamepadKey, "onAirVR"));

    IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
}

FAirVRServerInput::~FAirVRServerInput()
{
    IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);

    for (auto& Pair : PlayerInputMap) {
        delete Pair.Value;
    }
    PlayerInputMap.Empty();
}

void FAirVRServerInput::Tick(float DeltaTime)
{
    TArray<int32> CurrentPlayers;
    UAirVRServerFunctionLibrary::GetCurrentPlayers(CurrentPlayers);

    TArray<int32> KeysToRemove;
    PlayerInputMap.GetKeys(KeysToRemove);

    for (auto& Player : CurrentPlayers) {
        if (PlayerInputMap.Contains(Player)) {
            KeysToRemove.Remove(Player);
        }
        else {
            PlayerInputMap.Add(Player, new FAirVRCameraRigInput(Player));
        }
    }

    for (auto& Key : KeysToRemove) {
        FAirVRCameraRigInput* Input = nullptr;
        PlayerInputMap.RemoveAndCopyValue(Key, Input);

        check(Input);
        delete Input;
    }
}

void FAirVRServerInput::SendControllerEvents()
{
    for (auto& PairIt : PlayerInputMap) {
        PairIt.Value->UpdateAndDispatch(MessageHandler.Get());
    }
}

void FAirVRServerInput::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
    MessageHandler = InMessageHandler;
}

bool FAirVRServerInput::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
    return false;
}

void FAirVRServerInput::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
    // do nothing
}

void FAirVRServerInput::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values)
{
    check(IsInGameThread());

    if (PlayerInputMap.Contains(ControllerId)) {
        PlayerInputMap[ControllerId]->SetForceFeedback(Values);
    }
}

bool FAirVRServerInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMeterScale) const
{
    if (!IsInGameThread()) {
        // TODO: benefit tracking status on render thread by making player input map thread-safe
        // the return value does not have any effect.
        return false;
    }

    if (PlayerInputMap.Contains(ControllerIndex) &&
        PlayerInputMap[ControllerIndex]->IsControllerAvailable(DeviceHand)) {
        PlayerInputMap[ControllerIndex]->GetControllerOrientationAndPosition(DeviceHand, OutOrientation, OutPosition);
        return true;
    }
    return false;
}

ETrackingStatus FAirVRServerInput::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const 
{
    if (!IsInGameThread()) {
        // TODO: benefit tracking status on render thread by making player input map thread-safe
        // the return value does not have any effect.
        return ETrackingStatus::NotTracked;
    }

    if (PlayerInputMap.Contains(ControllerIndex) &&
        PlayerInputMap[ControllerIndex]->IsControllerAvailable(DeviceHand)) {
        return ETrackingStatus::Tracked;
    }
    return ETrackingStatus::NotTracked;
}

void FAirVRServerInput::SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) 
{
    check(IsInGameThread());

    if (PlayerInputMap.Contains(ControllerId) &&
        PlayerInputMap[ControllerId]->IsControllerAvailable((EControllerHand)Hand)) {
        PlayerInputMap[ControllerId]->SetHapticFeedback((EControllerHand)Hand, Values);
    }
}

void FAirVRServerInput::GetHapticFrequencyRange(float& MinFrequency, float &MaxFrequency) const 
{
    MinFrequency = 0.0f;
    MaxFrequency = 1.0f;
}

float FAirVRServerInput::GetHapticAmplitudeScale() const 
{
    return 1.0f;
}

#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 17)
FName FAirVRServerInput::GetMotionControllerDeviceTypeName() const
{
    return TEXT("AirVRServerInput");
}
#endif

class FAirVRServerInputPlugin : public IAirVRServerInputPlugin
{
public:
    virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override
    {
        return TSharedPtr<class IInputDevice>(new FAirVRServerInput(InMessageHandler));
    }
};

IMPLEMENT_MODULE(FAirVRServerInputPlugin, onAirVRServerInput)

#undef LOCTEXT_NAMESPACE
