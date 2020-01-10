/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "UObject/NoExportTypes.h"

#include "onairvr_server.h"

#include "AirVRClientConfig.h"
#include "AirVRClientConfigImpl.generated.h"

UCLASS()
class UAirVRClientConfigImpl : public UAirVRClientConfig
{
	GENERATED_BODY()

public:
    UAirVRClientConfigImpl();
	
public:
    void SetConfig(const ONAIRVR_CLIENT_CONFIG& Config, float WorldToMeters);
};
