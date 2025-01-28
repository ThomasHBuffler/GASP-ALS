// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class AdditionalBlueprintNodes : ModuleRules
	{
        public AdditionalBlueprintNodes(ReadOnlyTargetRules target) : base(target)
        {
	    
            PrivateDependencyModuleNames.AddRange(
				new string[] {
                    "Core",
					"CoreUObject",
                    "Engine",
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