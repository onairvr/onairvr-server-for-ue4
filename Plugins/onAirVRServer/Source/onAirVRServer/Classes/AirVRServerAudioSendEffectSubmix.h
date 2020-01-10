/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Sound/SoundEffectSubmix.h"
#include "AirVRServerAudioSendEffectSubmix.generated.h"

USTRUCT(BlueprintType)
struct FSubmixEffectSubmixAirVRServerAudioSendSettings
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset")
    int32 PlayerControllerID;

    FSubmixEffectSubmixAirVRServerAudioSendSettings() : PlayerControllerID(-1) {}
};

class ONAIRVRSERVER_API FSubmixEffectSubmixAirVRServerAudioSend : public FSoundEffectSubmix
{
public:
    FSubmixEffectSubmixAirVRServerAudioSend();

public:
    // implements FSoundEffectSubmix interfaces
    virtual void Init(const FSoundEffectSubmixInitData& InSampleRate) override;
    virtual void OnPresetChanged() override;
    virtual uint32 GetDesiredInputChannelCountOverride() const override;
    virtual void OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData) override;

private:
    class FAirVRServerHMD* GetHMD() const;

private:
    float SampleRate;
};

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class ONAIRVRSERVER_API USubmixEffectSubmixAirVRServerAudioSendPreset : public USoundEffectSubmixPreset
{
    GENERATED_BODY()

public:
    EFFECT_PRESET_METHODS(SubmixEffectSubmixAirVRServerAudioSend)

    UFUNCTION(BlueprintCallable, Category = "Audio|Effects")
    void SetSettings(const FSubmixEffectSubmixAirVRServerAudioSendSettings& InSettings);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SubmixEffectPreset)
    FSubmixEffectSubmixAirVRServerAudioSendSettings Settings;
};
