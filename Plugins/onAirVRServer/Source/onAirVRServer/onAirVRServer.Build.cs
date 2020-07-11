/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

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
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "Classes")
            });

            PrivateIncludePaths.AddRange(new string[] {
                Path.Combine(ModuleDirectory, "Private"),
                Path.Combine(ModuleDirectory, "../onAirVRServerInput/Private"),
                Path.Combine(Path.GetFullPath(Target.RelativeEnginePath), "Source/Runtime/Windows/D3D11RHI/Public"),
                Path.Combine(Path.GetFullPath(Target.RelativeEnginePath), "Source/Runtime/Windows/D3D11RHI/Private/Windows"),
                Path.Combine(Path.GetFullPath(Target.RelativeEnginePath), "Source/Runtime/Renderer/Private"),
                Path.Combine(Path.GetFullPath(Target.RelativeEnginePath), "Source/Runtime/AudioMixer/Private")
            });

            PrivateDependencyModuleNames.AddRange(new string[] {
                "Core",
                "Engine",
                "CoreUObject",
                "Projects",
                "InputCore",
                "RenderCore",
                "Renderer",
                "RHI",
                "D3D11RHI",
                "HeadMountedDisplay",
                "AudioMixer",
                "Json",
                "HTTP"
            });

            if (Target.bBuildEditor) {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }

            string BinariesPath = Path.Combine(ModuleDirectory, "..", "..", "Binaries", "Win64");
            string OCSDllName = "ocs.dll";
            string OCSDllPath = Path.Combine(BinariesPath, OCSDllName);
            string OCSLibPath = Path.Combine(BinariesPath, "ocs.lib");

            PublicAdditionalLibraries.Add(OCSLibPath);

            PublicDelayLoadDLLs.Add(OCSDllName);
            RuntimeDependencies.Add(OCSDllPath);
        }
    }
}
