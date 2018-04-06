/***********************************************************

  Copyright (c) 2017-2018 Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

using UnrealBuildTool;
using System.Collections.Generic;

public class onAirVRUnrealSampleTarget : TargetRules
{
	public onAirVRUnrealSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

        ExtraModuleNames.AddRange(new string[] { "onAirVRUnrealSample" });
    }
}
