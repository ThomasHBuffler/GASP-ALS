// Fill out your copyright notice in the Description page of Project Settings.


#include "EnhancedSettingsEngineSubsystem.h"

#include "EnhancedSetting.h"
#include "EnhancedSettingsTypes.h"
#include "EnhancedSettingsContainer.h"
#include "EnhancedSettingsGameInstanceSubsystem.h"
#include "EnhancedSettingsLocalPlayerSubsystem.h"

TMap<ULocalPlayer*, UEnhancedSettingsLocalPlayerSubsystem*> UEnhancedSettingsEngineSubsystem::LocalPlayerSubsystems;
UEnhancedSettingsGameInstanceSubsystem* UEnhancedSettingsEngineSubsystem::EnhancedSettingsGameInstanceSubsystem = nullptr;

bool UEnhancedSettingsEngineSubsystem::GetBoundBoolValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope()))
	{
		return Container->GetBoundBoolValue(nullptr, Setting);
	}
	
	return false;
}

int UEnhancedSettingsEngineSubsystem::GetBoundIntValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope()))
	{
		return Container->GetBoundIntValue(nullptr, Setting);
	}
	
	return -1;
}

float UEnhancedSettingsEngineSubsystem::GetBoundFloatValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope()))
	{
		return Container->GetBoundFloatValue(nullptr, Setting);
	}

	return -1.0f;
}

FColor UEnhancedSettingsEngineSubsystem::GetBoundColorValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope()))
	{
		return Container->GetBoundColorValue(nullptr, Setting);
	}
	
	return FColor::Red;
}

FGameplayTag UEnhancedSettingsEngineSubsystem::GetBoundTagValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope()))
	{
		return Container->GetBoundTagValue(nullptr, Setting);
	}
	
	return FGameplayTag();
}

void UEnhancedSettingsEngineSubsystem::ChangeBoundBoolValue(const APlayerController* PlayerController,
	UEnhancedSetting* Setting, bool Value)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope())) Container->ChangeBoundBoolValue(Setting, Value);
}

void UEnhancedSettingsEngineSubsystem::ChangeBoundIntValue(const APlayerController* PlayerController,
	UEnhancedSetting* Setting, int Value)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope())) Container->ChangeBoundIntValue(Setting, Value);
}

void UEnhancedSettingsEngineSubsystem::ChangeBoundFloatValue(const APlayerController* PlayerController,
	UEnhancedSetting* Setting, float Value)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope())) Container->ChangeBoundFloatValue(Setting, Value);
}

void UEnhancedSettingsEngineSubsystem::ChangeBoundColorValue(const APlayerController* PlayerController,
	UEnhancedSetting* Setting, FColor Value)
{
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope())) Container->ChangeBoundColorValue(Setting, Value);
}

void UEnhancedSettingsEngineSubsystem::ChangeBoundTagValue(const APlayerController* PlayerController,
	UEnhancedSetting* Setting, FGameplayTag Value)
{
	if (!IsValid(PlayerController)) return;
	
	if (UEnhancedSettingsContainer* Container = GetEnhancedSettingsContainerFromScope(PlayerController, Setting->GetEnhancedSettingScope())) Container->ChangeBoundTagValue(Setting, Value);
}

void UEnhancedSettingsEngineSubsystem::ApplyChanges(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return;
	
	for (const auto& SettingsContainer :GetEnhancedSettingsContainersForPlayer(PlayerController))
	{
		SettingsContainer->ApplyChanges();
	}
}

void UEnhancedSettingsEngineSubsystem::ClearChanges(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return;
	
	for (const auto& SettingsContainer :GetEnhancedSettingsContainersForPlayer(PlayerController))
	{
		
		SettingsContainer->ClearUnappliedChanges();
	}
}

bool UEnhancedSettingsEngineSubsystem::HasUnappliedChanges(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return false;
	
	for (const auto& SettingsContainer :GetEnhancedSettingsContainersForPlayer(PlayerController))
	{
		if (SettingsContainer->HasUnappliedChanges()) return true;
	}
	return false;
}

bool UEnhancedSettingsEngineSubsystem::CanResetSettings(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return false;
	
	for (const auto& SettingsContainer :GetEnhancedSettingsContainersForPlayer(PlayerController))
	{
		if (SettingsContainer->CanResetSettings()) return true;
	}
	return false;
}

void UEnhancedSettingsEngineSubsystem::ResetSettings(const APlayerController* PlayerController, EEnhancedSettingScope Scope)
{
	if (!IsValid(PlayerController)) return;
	
	for (const auto& SettingsContainer :GetEnhancedSettingsContainersForPlayer(PlayerController))
	{
		SettingsContainer->ResetSettingsAndApply();
	}
}

void UEnhancedSettingsEngineSubsystem::AddEnhancedSettingWrapperChangedSetting(const APlayerController* PlayerController, UEnhancedSettingWrapper* Setting)
{
	if(UEnhancedSettingsContainer* SettingsContainer = GetEnhancedSettingsContainerFromScope(PlayerController, GameInstance))
	{
		SettingsContainer->AddEnhancedSettingWrapperChangedSetting(Setting);
	}
}

void UEnhancedSettingsEngineSubsystem::RemoveEnhancedSettingWrapperChangedSetting(const APlayerController* PlayerController, UEnhancedSettingWrapper* Setting)
{
	if(UEnhancedSettingsContainer* SettingsContainer = GetEnhancedSettingsContainerFromScope(PlayerController, GameInstance))
	{
		SettingsContainer->RemoveEnhancedSettingWrapperChangedSetting(Setting);
	}
}

void UEnhancedSettingsEngineSubsystem::AddPendingEnhancedSettingWrapper(const APlayerController* PlayerController,
	UEnhancedSettingWrapper* Setting)
{
	if(UEnhancedSettingsContainer* SettingsContainer = GetEnhancedSettingsContainerFromScope(PlayerController, GameInstance))
	{
		SettingsContainer->AddPendingEnhancedSettingWrapper(Setting);
	}
}

void UEnhancedSettingsEngineSubsystem::RemovePendingEnhancedSettingWrapper(const APlayerController* PlayerController,
	UEnhancedSettingWrapper* Setting)
{
	if(UEnhancedSettingsContainer* SettingsContainer = GetEnhancedSettingsContainerFromScope(PlayerController, GameInstance))
	{
		SettingsContainer->RemovePendingEnhancedSettingWrapper(Setting);
	}
}

void UEnhancedSettingsEngineSubsystem::RegisterLocalPlayerSubsystem(UEnhancedSettingsLocalPlayerSubsystem* InEnhancedSettingsLocalPlayerSubsystem)
{
	LocalPlayerSubsystems.Add(InEnhancedSettingsLocalPlayerSubsystem->GetLocalPlayer(), InEnhancedSettingsLocalPlayerSubsystem);
}

void UEnhancedSettingsEngineSubsystem::UnregisterLocalPlayerSubsystem(const UEnhancedSettingsLocalPlayerSubsystem* InEnhancedSettingsLocalPlayerSubsystem)
{
	LocalPlayerSubsystems.Remove(InEnhancedSettingsLocalPlayerSubsystem->GetLocalPlayer());
}

void UEnhancedSettingsEngineSubsystem::RegisterGameInstanceSubsystem(
	UEnhancedSettingsGameInstanceSubsystem* InEnhancedSettingsGameInstanceSubsystem)
{
	EnhancedSettingsGameInstanceSubsystem = InEnhancedSettingsGameInstanceSubsystem;
}

void UEnhancedSettingsEngineSubsystem::UnregisterGameInstanceSubsystem()
{
	EnhancedSettingsGameInstanceSubsystem = nullptr;
}

TArray<UEnhancedSettingsContainer*> UEnhancedSettingsEngineSubsystem::GetAllEnhancedSettingsContainers()
{
	return TArray<UEnhancedSettingsContainer*>();
}

TArray<UEnhancedSettingsContainer*> UEnhancedSettingsEngineSubsystem::GetEnhancedSettingsContainersForPlayer(
	const APlayerController* PlayerController)
{
	TArray<UEnhancedSettingsContainer*> SettingsContainers;
	
	if (!IsValid(PlayerController)) return SettingsContainers;
	SettingsContainers.Add(GetEnhancedSettingsContainerFromScope(PlayerController, LocalPlayer));
	SettingsContainers.Add(GetEnhancedSettingsContainerFromScope(PlayerController, GameInstance));
	return SettingsContainers;
}

UEnhancedSettingsContainer* UEnhancedSettingsEngineSubsystem::GetEnhancedSettingsContainerFromScope(const APlayerController* PlayerController, EEnhancedSettingScope Scope)
{
	switch (Scope)
	{
		default:
			checkf(false, TEXT("ERROR, Unhandled setting scope"));break;

		case GameInstance:
			return EnhancedSettingsGameInstanceSubsystem->GetEnhancedSettingsContainer();

		case LocalPlayer:
			return LocalPlayerSubsystems[PlayerController->GetLocalPlayer()]->GetEnhancedSettingsContainer();
	}
	
	return nullptr;
}
