// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "EnhancedSettingsContainer.h"
#include "EnhancedSettingsEngineSubsystem.generated.h"


class UEnhancedSettingsGameInstanceSubsystem;
class UEnhancedSettingsLocalPlayerSubsystem;

UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingsEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

protected:
	static TMap<ULocalPlayer*, UEnhancedSettingsLocalPlayerSubsystem*> LocalPlayerSubsystems;

	static UEnhancedSettingsGameInstanceSubsystem* EnhancedSettingsGameInstanceSubsystem;
public:	
	// Getters
	UFUNCTION(BlueprintPure, Category="Enhanced Settings")
	static bool GetBoundBoolValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting);
	UFUNCTION(BlueprintPure, Category="Enhanced Settings")
	static int GetBoundIntValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting);
	UFUNCTION(BlueprintPure, Category="Enhanced Settings")
	static float GetBoundFloatValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting);
	UFUNCTION(BlueprintPure, Category="Enhanced Settings")
	static FColor GetBoundColorValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting);
	UFUNCTION(BlueprintPure, Category="Enhanced Settings")
	static FGameplayTag GetBoundTagValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting);	

	// Setters
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ChangeBoundBoolValue(const APlayerController* PlayerController, UEnhancedSetting* Setting, bool Value);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ChangeBoundIntValue(const APlayerController* PlayerController, UEnhancedSetting* Setting, int Value);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ChangeBoundFloatValue(const APlayerController* PlayerController, UEnhancedSetting* Setting, float Value);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ChangeBoundColorValue(const APlayerController* PlayerController, UEnhancedSetting* Setting, FColor Value);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ChangeBoundTagValue(const APlayerController* PlayerController, UEnhancedSetting* Setting, FGameplayTag Value);

	// Utility
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ApplyChanges(const APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ClearChanges(const APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static bool HasUnappliedChanges(const APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static bool CanResetSettings(const APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	static void ResetSettings(const APlayerController* PlayerController, EEnhancedSettingScope Scope);
	
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	void AddEnhancedSettingWrapperChangedSetting(const APlayerController* PlayerController, UEnhancedSettingWrapper* Setting);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	void RemoveEnhancedSettingWrapperChangedSetting(const APlayerController* PlayerController, UEnhancedSettingWrapper* Setting);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	void AddPendingEnhancedSettingWrapper(const APlayerController* PlayerController,UEnhancedSettingWrapper* Setting);
	UFUNCTION(BlueprintCallable, Category="Enhanced Settings")
	void RemovePendingEnhancedSettingWrapper(const APlayerController* PlayerController,UEnhancedSettingWrapper* Setting);
	
	static void RegisterLocalPlayerSubsystem(UEnhancedSettingsLocalPlayerSubsystem* InEnhancedSettingsLocalPlayerSubsystem);
	static void UnregisterLocalPlayerSubsystem(const UEnhancedSettingsLocalPlayerSubsystem* InEnhancedSettingsLocalPlayerSubsystem);
	static void RegisterGameInstanceSubsystem(UEnhancedSettingsGameInstanceSubsystem* InEnhancedSettingsGameInstanceSubsystem);
	static void UnregisterGameInstanceSubsystem();

	static TArray<UEnhancedSettingsContainer*> GetAllEnhancedSettingsContainers();
	static TArray<UEnhancedSettingsContainer*> GetEnhancedSettingsContainersForPlayer(const APlayerController* PlayerController);
	static UEnhancedSettingsContainer* GetEnhancedSettingsContainerFromScope(const APlayerController* PlayerController, EEnhancedSettingScope Scope);
};
