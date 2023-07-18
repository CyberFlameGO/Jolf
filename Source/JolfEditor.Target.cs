// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;
using System.Collections.Generic;

public class JolfEditorTarget : TargetRules
{
	public JolfEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Jolf", "JolfWidgets", "JolfEditor", "JolfTests" } );
	}
}
