// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;

public class JolfUMG : ModuleRules
{
	public JolfUMG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Slate",
				"SlateCore",
				"InputCore",
				"UMG",
			}
			);

		PrivateDependencyModuleNames.AddRange(new string[] { "JolfSlate", "Engine" });
	}
}
