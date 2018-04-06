/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class IAirVRServerEditorModule : public IModuleInterface
{
public:
    static inline IAirVRServerEditorModule& Get()
    {
        return FModuleManager::LoadModuleChecked<IAirVRServerEditorModule>("AirVRServerEditorModule");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("AirVRServerEditorModule");
    }
};
