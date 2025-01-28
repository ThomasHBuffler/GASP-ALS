// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EnhancedSettings : ModuleRules
{
	public EnhancedSettings(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] { });
				
		
		PrivateIncludePaths.AddRange(
			new string[] { });
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"EnhancedInput"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"GameplayTags",
				"Json",            
				"JsonUtilities",   
				"ToolMenus", "UMG"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[] { });
	}
}
