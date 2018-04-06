/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "AirVRClientConfig.generated.h"

UENUM()
enum class EAirVRClientType : uint8
{
    None,
    Monoscopic,
    Stereoscopic
};

UCLASS()
class ONAIRVRSERVER_API UAirVRClientConfig : public UObject
{
    GENERATED_BODY() 

public:
    UPROPERTY(BlueprintReadOnly, Category = "onAirVR")
    EAirVRClientType ClientType;

    UPROPERTY(BlueprintReadOnly, Category = "onAirVR")
    int32 VideoWidth;

    UPROPERTY(BlueprintReadOnly, Category = "onAirVR")
    int32 VideoHeight;

    UPROPERTY(BlueprintReadOnly, Category = "onAirVR")
    float FrameRate;

    UPROPERTY(BlueprintReadOnly, Category = "onAirVR")
    float FOV;

    UPROPERTY(BlueprintReadWrite, Category = "onAirVR")
    float IPD;

    UPROPERTY(BlueprintReadWrite, Category = "onAirVR")
    FVector EyeCenterPosition;

    UPROPERTY(BlueprintReadOnly, Category = "onAirVR")
    FString UserID;
};
