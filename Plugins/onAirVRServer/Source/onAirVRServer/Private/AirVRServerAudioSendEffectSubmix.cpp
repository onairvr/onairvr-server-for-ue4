/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerAudioSendEffectSubmix.h"
#include "AirVRServerPrivate.h"

#include "AirVRServerHMD.h"

FSubmixEffectSubmixAirVRServerAudioSend::FSubmixEffectSubmixAirVRServerAudioSend()
{}

void FSubmixEffectSubmixAirVRServerAudioSend::Init(const FSoundEffectSubmixInitData& InSampleRate)
{
    SampleRate = InSampleRate.SampleRate;
}

void FSubmixEffectSubmixAirVRServerAudioSend::OnPresetChanged()
{
    // do nothing
}

uint32 FSubmixEffectSubmixAirVRServerAudioSend::GetDesiredInputChannelCountOverride() const
{
    return 2;
}

void FSubmixEffectSubmixAirVRServerAudioSend::OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData)
{
    check(InData.NumChannels == 2 && OutData.NumChannels >= 2);

    GET_EFFECT_SETTINGS(SubmixEffectSubmixAirVRServerAudioSend);

    Audio::AlignedFloatBuffer& InBuffer = *InData.AudioBuffer;
    Audio::AlignedFloatBuffer& OutBuffer = *OutData.AudioBuffer;

    if (FAirVRServerHMD* HMD = GetHMD()) {
        if (Settings.PlayerControllerID < 0) {
            HMD->SendAudioFrameToAllPlayers(InBuffer.GetData(), InData.NumFrames, InData.NumChannels, InData.AudioClock);
        }
        else {
            HMD->SendAudioFrame(Settings.PlayerControllerID, InBuffer.GetData(), InData.NumFrames, InData.NumChannels, InData.AudioClock);
        }
    }

    for (int32 Frame = 0; Frame < InData.NumFrames; ++Frame) {
        for (int32 Channel = 0; Channel < InData.NumChannels; ++Channel) {
            OutBuffer[Frame * OutData.NumChannels + Channel] = InBuffer[Frame * InData.NumChannels + Channel];
        }
    }
}

FAirVRServerHMD* FSubmixEffectSubmixAirVRServerAudioSend::GetHMD() const
{
    static FName SystemName(TEXT("onAirVRServer"));
    if (GEngine->XRSystem.IsValid() && GEngine->XRSystem.Get()->GetSystemName() == SystemName) {
        return static_cast<FAirVRServerHMD*>(GEngine->XRSystem.Get());
    }
    return nullptr;
}

void USubmixEffectSubmixAirVRServerAudioSendPreset::SetSettings(const FSubmixEffectSubmixAirVRServerAudioSendSettings& InSettings)
{
    UpdateSettings(InSettings);
}
