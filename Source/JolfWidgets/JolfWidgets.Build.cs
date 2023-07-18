// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;

public class JolfWidgets : ModuleRules
{
	public JolfWidgets(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Jolf", "JolfUMG", "Slate", "JolfSlate", "SlateCore" });
    }
}
