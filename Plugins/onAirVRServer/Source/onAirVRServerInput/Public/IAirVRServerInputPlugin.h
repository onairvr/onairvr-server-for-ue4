/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IInputDeviceModule.h"

class IAirVRServerInputPlugin : public IInputDeviceModule
{
public:
    static inline IAirVRServerInputPlugin& Get()
    {
        return FModuleManager::LoadModuleChecked<IAirVRServerInputPlugin>("onAirVRServerInput");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("onAirVRServerInput");
    }
};
