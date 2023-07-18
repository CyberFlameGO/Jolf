// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;

public class JolfEditor : ModuleRules
{
	public JolfEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PrivateDependencyModuleNames.AddRange(new string[] { "Jolf", "Core", "CoreUObject", "Engine", "AssetTools" });
    }
}
