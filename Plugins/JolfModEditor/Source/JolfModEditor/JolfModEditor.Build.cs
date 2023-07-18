// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;

public class JolfModEditor : ModuleRules
{
	public JolfModEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"EditorStyle",
				"DeveloperSettings",
				"PluginBrowser",
				"Projects",
				"Slate",
				"SlateCore",
				"UATHelper"
			}
			);
	}
}
