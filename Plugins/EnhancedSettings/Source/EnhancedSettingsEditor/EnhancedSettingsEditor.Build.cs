// Copyright Epic Games, Inc. All Rights Reserved.

using EpicGames.Core;

namespace UnrealBuildTool.Rules
{
	public class EnhancedSettingsEditor : ModuleRules
	{
        public EnhancedSettingsEditor(ReadOnlyTargetRules target) : base(target)
        {
	        
			PublicDependencyModuleNames.AddRange(
				new string[] {
					"EnhancedSettings",
                }
            );
            PrivateDependencyModuleNames.AddRange(
	            new string[] {
		            "BlueprintGraph",
		            "Core",
		            "CoreUObject",
		            "Engine",
		            "GraphEditor",
		            "InputCore",
		            "KismetCompiler",
		            "PropertyEditor",
		            "Slate",
		            "SlateCore",
		            "UnrealEd",
		            "GameplayTags", 
		            "GameplayTagsEditor",  
		            "AssetTools",
	            }
            );
        }
    }
}