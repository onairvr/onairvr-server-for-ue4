/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

#include "AirVRServerEditorPrivate.h"

#include "AirVRServerSettings.h"
#include "ISettingsModule.h"

class FAirVRServerEditorModule : public IAirVRServerEditorModule
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FAirVRServerEditorModule, onAirVRServerEditor);

void FAirVRServerEditorModule::StartupModule()
{
    ISettingsModule* SettingsModule = FModuleManager::Get().GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule)
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "onAirVR Server", NSLOCTEXT("onAirVRServer", "onAirVR Server", "onAirVR Server"),
            NSLOCTEXT("onAirVRServer", "Configure onAirVR Server settings", "Configure onAirVR Server settings"), GetMutableDefault<UAirVRServerSettings>());
    }
}

void FAirVRServerEditorModule::ShutdownModule()
{
    // do nothing
}
