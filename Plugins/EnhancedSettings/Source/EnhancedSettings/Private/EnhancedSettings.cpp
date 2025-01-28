// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnhancedSettings.h"
#include "GameplayTagsManager.h"

#define LOCTEXT_NAMESPACE "FEnhancedSettingsModule"

void FEnhancedSettingsModule::StartupModule()
{
	UGameplayTagsManager::Get().AddTagIniSearchPath(FPaths::ProjectPluginsDir() / TEXT("EnhancedSettings/Config/Tags"));
}

void FEnhancedSettingsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEnhancedSettingsModule, EnhancedSettings)