/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

using System.IO;

namespace UnrealBuildTool.Rules 
{
    public class onAirVRServer : ModuleRules 
    {
        public onAirVRServer(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(new string[] {
                "onAirVRServer/Public",
                "onAirVRServer/Classes",
            });

            PrivateIncludePaths.AddRange(new string[] {
                "onAirVRServer/Private",
                "onAirVRServerInput/Private",
                "Runtime/Windows/D3D11RHI/Public",
                "Runtime/Windows/D3D11RHI/Private/Windows",
                "C:/Program Files/Epic Games/UE_4.20/Engine/Source/Runtime/Renderer/Private"
            });


            PrivateDependencyModuleNames.AddRange(new string[] {
                "Core",
                "Engine",
                "CoreUObject",
                "Projects",
                "InputCore",
                "RenderCore",
                "ShaderCore",
                "Renderer",
                "RHI",
                "D3D11RHI",
                "UtilityShaders",
                "HeadMountedDisplay",
                "AudioMixer",
                "Json",
                "HTTP"
            });

            if (Target.bBuildEditor) {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }

            string BinariesPath = Path.Combine(ModuleDirectory, "..", "..", "Binaries", "Win64");
            string AirVRServerDllName = "onAirVRUnrealServerPlugin.dll";
            string AirVRServerDllPath = Path.Combine(BinariesPath, AirVRServerDllName);
            string AirVRServerLibPath = Path.Combine(BinariesPath, "onAirVRUnrealServerPlugin.lib");

            PublicAdditionalLibraries.Add(AirVRServerLibPath);

            PublicDelayLoadDLLs.Add(AirVRServerDllName);
            RuntimeDependencies.Add(AirVRServerDllPath);
        }
    }
}
