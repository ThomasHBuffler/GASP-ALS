// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class EnhancedSettingsBlueprintNodes : ModuleRules
	{
        public EnhancedSettingsBlueprintNodes(ReadOnlyTargetRules target) : base(target)
        {
	        PublicDependencyModuleNames.AddRange(new string[] { "EnhancedSettingsEditor", });
	        
            PrivateDependencyModuleNames.AddRange(
				new string[] {
                    "Core",
					"CoreUObject",
                    "Engine",
					"EnhancedSettings",
					"GameplayTags"
                }
            );  
            
            if (target.bBuildEditor)
            {
	            PrivateDependencyModuleNames.AddRange(new string[] { 
		            "UnrealEd",
		            "BlueprintGraph", 
		            "GraphEditor", 
		            "KismetCompiler",
		            "PropertyEditor",
		            "Slate",
		            "SlateCore",
		            
	            });
            }
        }
    }
}