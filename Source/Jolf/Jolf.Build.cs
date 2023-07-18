// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

using UnrealBuildTool;

public class Jolf : ModuleRules
{
	public Jolf(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "PhysicsCore", "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "Landscape", "JolfSceneProbe", "Projects", "JolfSlate" });
    }
}
