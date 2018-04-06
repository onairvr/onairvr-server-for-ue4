/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AirVRSampleHelperFunctionLibrary.generated.h"

UCLASS()
class UAirVRSampleHelperFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "AirVRSample")
    static void GetPlayerMoveInputDirection(APlayerController* PlayerController, FVector2D& OutResult);
};
