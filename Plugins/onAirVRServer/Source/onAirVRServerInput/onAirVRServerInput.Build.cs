/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

namespace UnrealBuildTool.Rules 
{
    public class onAirVRServerInput : ModuleRules 
    {
        public onAirVRServerInput(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(new string[] {
                "onAirVRServer/Public",
                "onAirVRServer/Classes"
            });

            PrivateIncludePaths.AddRange(new string[] {
                "onAirVRServer/Private",
            });

            PrivateDependencyModuleNames.AddRange(new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputDevice",
                "InputCore",
                "HeadMountedDisplay",
                "UMG",
                "onAirVRServer"
            });
        }
    }
}
