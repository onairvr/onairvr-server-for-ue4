/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "ModuleManager.h"
#include "IHeadMountedDisplayModule.h"

class IAirVRServerPlugin : public IHeadMountedDisplayModule
{
public:
    static inline IAirVRServerPlugin& Get()
    {
        return FModuleManager::LoadModuleChecked<IAirVRServerPlugin>("onAirVRServer");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("onAirVRServer");
    }
};