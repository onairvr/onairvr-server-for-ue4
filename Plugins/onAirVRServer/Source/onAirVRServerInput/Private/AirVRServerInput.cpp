/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerInput.h"
#include "AirVRServerInputPrivate.h"

#include "AirVRServerFunctionLibrary.h"
#include "AirVRCameraRigInput.h"
#include "IInputDevice.h"
#include "Internationalization/internationalization.h"
#include "IAirVRServerPlugin.h"
#include "XRMotionControllerBase.h"
#include "GenericPlatform/IInputInterface.h"

#define LOCTEXT_NAMESPACE "onAirVRServer"

const FKey FAirVRInputKey::TouchpadPositionX("AirVR_Touchpad_PositionX");
const FKey FAirVRInputKey::TouchpadPositionY("AirVR_Touchpad_PositionY");
const FKey FAirVRInputKey::TouchpadTouch("AirVR_Touchpad_Touch");
const FKey FAirVRInputKey::TouchpadBackButton("AirVR_Touchpad_BackButton");
const FKey FAirVRInputKey::TouchpadUp("AirVR_Touchpad_Up");
const FKey FAirVRInputKey::TouchpadDown("AirVR_Touchpad_Down");
const FKey FAirVRInputKey::TouchpadLeft("AirVR_Touchpad_Left");
const FKey FAirVRInputKey::TouchpadRight("AirVR_Touchpad_Right");
const FKey FAirVRInputKey::TrackedControllerTouchpadTouch("AirVR_Controller_Touch");

class FAirVRServerInput : public IInputDevice, public FXRMotionControllerBase
{
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
    virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;

    // implements IMotionController pure virtual methods
    virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMeterScale) const override;
    virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override;

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
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadPositionX, LOCTEXT("AirVR_Touchpad_PositionX", "onAirVR Touchpad Position X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadPositionY, LOCTEXT("AirVR_Touchpad_PositionY", "onAirVR Touchpad Position Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadTouch, LOCTEXT("AirVR_Touchpad_Touch", "onAirVR Touchpad Touch"), FKeyDetails::GamepadKey));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadBackButton, LOCTEXT("AirVR_Touchpad_BackButton", "onAirVR Touchpad Back Button"), FKeyDetails::GamepadKey));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadUp, LOCTEXT("AirVR_Touchpad_Up", "onAirVR Touchpad Up"), FKeyDetails::GamepadKey));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadDown, LOCTEXT("AirVR_Touchpad_Down", "onAirVR Touchpad Down"), FKeyDetails::GamepadKey));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadLeft, LOCTEXT("AirVR_Touchpad_Left", "onAirVR Touchpad Left"), FKeyDetails::GamepadKey));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TouchpadRight, LOCTEXT("AirVR_Touchpad_Right", "onAirVR Touchpad Right"), FKeyDetails::GamepadKey));
    EKeys::AddKey(FKeyDetails(FAirVRInputKey::TrackedControllerTouchpadTouch, LOCTEXT("AirVR_Controller_Touch", "onAirVR Tracked Controller Touchpad Touch"), FKeyDetails::GamepadKey));

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
    // do nothing (do not support force feedback)
}

void FAirVRServerInput::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{
    // do nothing (do not support force feedback)
}

bool FAirVRServerInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMeterScale) const
{
    if (PlayerInputMap.Contains(ControllerIndex)) {
        PlayerInputMap[ControllerIndex]->GetControllerOrientationAndPosition(OutOrientation, OutPosition);
        //OutPosition *= WorldToMeterScale;
        return true;
    }
    return false;
}

ETrackingStatus FAirVRServerInput::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
    // TODO
    return ETrackingStatus::Tracked;
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
