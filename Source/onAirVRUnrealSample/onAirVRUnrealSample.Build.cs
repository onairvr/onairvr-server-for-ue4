/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

namespace UnrealBuildTool.Rules {
    public class onAirVRUnrealSample : ModuleRules {
        public onAirVRUnrealSample(ReadOnlyTargetRules Target) : base(Target) {
            PublicIncludePaths.AddRange(new string[] { "onAirVRServer/Public", "onAirVRServer/Classes", "onAirVRServerInput/Public" });
            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "onAirVRServerInput" });

            PrivateDependencyModuleNames.AddRange(new string[] { });


            // Uncomment if you are using Slate UI
            // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

            // Uncomment if you are using online features
            // PrivateDependencyModuleNames.Add("OnlineSubsystem");

            // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        }
    }
}
