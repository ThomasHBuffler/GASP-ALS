// Fill out your copyright notice in the Description page of Project Settings.


#include "EnhancedSettingsLocalPlayerSubsystem.h"

#include "EnhancedSettingsContainer.h"
#include "EnhancedSettingsEngineSubsystem.h"


UEnhancedSettingsLocalPlayerSubsystem::UEnhancedSettingsLocalPlayerSubsystem()
{
	EnhancedSettingsContainer = CreateDefaultSubobject<UEnhancedSettingsContainer>(TEXT("EnhancedSettingsContainer"));
}

void UEnhancedSettingsLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	EnhancedSettingsContainer->Initialize();

	// Register this subsystem with the Enhanced Settings Engine Subsystem
	UEnhancedSettingsEngineSubsystem* EnhancedSettingsEngineSubsystem = GEngine->GetEngineSubsystem<UEnhancedSettingsEngineSubsystem>();
	checkf(EnhancedSettingsEngineSubsystem, TEXT("Enhanced Settings Engine Subsystem not found!"));
	{
		EnhancedSettingsEngineSubsystem->RegisterLocalPlayerSubsystem(this);
	}
}

void UEnhancedSettingsLocalPlayerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UEnhancedSettingsEngineSubsystem* EnhancedSettingsEngineSubsystem = GEngine->GetEngineSubsystem<UEnhancedSettingsEngineSubsystem>();
	checkf(EnhancedSettingsEngineSubsystem, TEXT("Enhanced Settings Engine Subsystem not found!"));
	{
		EnhancedSettingsEngineSubsystem->UnregisterLocalPlayerSubsystem(this);
	}
}

void UEnhancedSettingsLocalPlayerSubsystem::SetProfile(const int Index) const
{
	EnhancedSettingsContainer->SetProfile(Index);
}

void UEnhancedSettingsLocalPlayerSubsystem::Tick(float DeltaTime)
{
	EnhancedSettingsContainer->Tick(DeltaTime);
}
