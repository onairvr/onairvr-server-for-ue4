/***********************************************************

  Copyright (c) 2017-present Clicked, Inc.

  Licensed under the MIT license found in the LICENSE file 
  in the Docs folder of the distributed package.

 ***********************************************************/

using UnrealBuildTool;
using System.Collections.Generic;

public class onAirVRUnrealSampleEditorTarget : TargetRules
{
	public onAirVRUnrealSampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

        ExtraModuleNames.AddRange(new string[] { "onAirVRUnrealSample" });
    }
}
