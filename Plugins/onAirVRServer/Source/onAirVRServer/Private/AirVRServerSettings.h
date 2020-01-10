/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRServerSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class ONAIRVRSERVER_API UAirVRServerSettings : public UObject
{
    GENERATED_BODY()

public:
    UAirVRServerSettings();

    UPROPERTY(GlobalConfig, EditAnywhere, Category = General, meta = (DisplayName = "License File", ToolTip = "the path of an onAirVR server license file"))
    FString LicenseFilePath;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Server", meta = (ClampMin = "1", DisplayName = "Max Client Count", ToolTip = "the maximum number of clients which can be connected simultaneouly"))
    uint32 MaxClientCount;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Server", meta = (DisplayName = "Port", ToolTip = "onAirVR server port number"))
    uint16 PortSTAP;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Video", meta = (DisplayName = "Video Bitrate (bps)", ToolTip = "bitrate in which to encode video for each client"))
    uint32 VideoBitrate;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Video", meta = (ClampMin = "1.0", ClampMax = "120.0", DisplayName = "Max Frame Rate", ToolTip = "maximum frame rate per second in which to encode video for each client"))
    float MaxFrameRate;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Video", meta = (ClampMin = "1.0", ClampMax = "120.0", DisplayName = "Default Frame Rate", ToolTip = "default frame rate per second in which to encode video for each client"))
    float DefaultFrameRate;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Audio", meta = (DisplayName = "Broadcast Audio To All Players", ToolTip = "if true, broadcast audio of master submix to all players. (UE4 AudioMixer feature required)"))
    bool BroadcastAudioToAllPlayers;

public:
    uint16 PortAMP;
    bool LoopbackOnlyForSTAP;
    FString UserData;
    FString GroupServer;

public:
    void ParseCommandLineArgs();
};
