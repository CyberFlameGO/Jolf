// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;

public class JolfSlate : ModuleRules
{
	public JolfSlate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Slate",
				"SlateCore",
				"InputCore"
			}
			);
	}
}
