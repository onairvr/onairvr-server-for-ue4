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

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Server", meta = (DisplayName = "Port", ToolTip = "onAirVR server port number"))
    uint16 PortSTAP;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Rendering", meta = (DisplayName = "Use Adaptive Frame Rate", ToolTip = "If true, application frame rate dynamically varies corresponding to the connected client"))
    bool AdaptiveFrameRate;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Rendering", meta = (ClampMin = "10", DisplayName = "Min Frame Rate", ToolTip = "The minimum frame rate when there is no connected client"))
    float MinFrameRate;

    UPROPERTY(GlobalConfig, EditAnywhere, Category = "Audio", meta = (DisplayName = "Disable Audio", ToolTip = "If true, do not send audio to clients (You can still send audio frame manually)"))
    bool DisableAudio;

    UPROPERTY(GlobalConfig)
    int MaxClientCount;

public:
    uint16 PortAMP;
    bool LoopbackOnlyForSTAP;
    FString UserData;
    FString GroupServer;

public:
    void ParseCommandLineArgs();
};
