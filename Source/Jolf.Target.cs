// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;
using System.Collections.Generic;

public class JolfTarget : TargetRules
{
	public JolfTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Jolf", "JolfWidgets" } );
	}
}
