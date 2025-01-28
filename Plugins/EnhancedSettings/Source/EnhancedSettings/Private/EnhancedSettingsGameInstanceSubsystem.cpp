// Copyright (c) 2024 Thomas Hawke Buffler. All rights reserved.This software and its accompanying documentation are the proprietary property of [Your Full Name]. Unauthorized copying, distribution, modification, or use of this software, in whole or in part, is strictly prohibited without prior written permission from the copyright owner.For inquiries, contact: ThomasHBufflerAccess@gmail.com


#include "EnhancedSettingsGameInstanceSubsystem.h"
#include "EnhancedSettingsContainer.h"
#include "EnhancedSettingsEngineSubsystem.h"

UEnhancedSettingsGameInstanceSubsystem::UEnhancedSettingsGameInstanceSubsystem()
{
	EnhancedSettingsContainer = CreateDefaultSubobject<UEnhancedSettingsContainer>(TEXT("EnhancedSettingsContainer"));
}

void UEnhancedSettingsGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	EnhancedSettingsContainer->Initialize();

	// Register this subsystem with the Enhanced Settings Engine Subsystem
	UEnhancedSettingsEngineSubsystem* EnhancedSettingsEngineSubsystem = GEngine->GetEngineSubsystem<UEnhancedSettingsEngineSubsystem>();
	checkf(EnhancedSettingsEngineSubsystem, TEXT("Enhanced Settings Engine Subsystem not found!"));
	{
		EnhancedSettingsEngineSubsystem->RegisterGameInstanceSubsystem(this);
	}
}

void UEnhancedSettingsGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UEnhancedSettingsEngineSubsystem* EnhancedSettingsEngineSubsystem = GEngine->GetEngineSubsystem<UEnhancedSettingsEngineSubsystem>();
	checkf(EnhancedSettingsEngineSubsystem, TEXT("Enhanced Settings Engine Subsystem not found!"));
	{
		EnhancedSettingsEngineSubsystem->UnregisterGameInstanceSubsystem();
	}
}