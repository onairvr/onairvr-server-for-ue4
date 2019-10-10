/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

using System.IO;

namespace UnrealBuildTool.Rules 
{
    public class onAirVRServerInput : ModuleRules 
    {
        public onAirVRServerInput(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(new string[] {
                Path.Combine(ModuleDirectory, "../onAirVRServer/Public"),
                Path.Combine(ModuleDirectory, "../onAirVRServer/Classes")
            });

            PrivateIncludePaths.AddRange(new string[] {
                Path.Combine(ModuleDirectory, "../onAirVRServer/Private")
            });

            PrivateDependencyModuleNames.AddRange(new string[] {
                "ApplicationCore",
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
