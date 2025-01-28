// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedSettingsContainer.h"
#include "EnhancedSettingsContainerAccessor.h"
#include "GameFramework/GameUserSettings.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "EnhancedSettingsLocalPlayerSubsystem.generated.h"

class USettingsContainer;

UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingsLocalPlayerSubsystem : public ULocalPlayerSubsystem, public IEnhancedSettingsContainerAccessor
{
	GENERATED_BODY()

	UEnhancedSettingsLocalPlayerSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void SetProfile(const int Index) const;
	
	UPROPERTY()
	TObjectPtr<UEnhancedSettingsContainer> EnhancedSettingsContainer;
	
public:
	// IEnhancedSettingsContainerAccessor interface
	virtual UEnhancedSettingsContainer* GetEnhancedSettingsContainer() const override
	{
		return EnhancedSettingsContainer;
	}
	
	virtual EEnhancedSettingScope GetEnhancedSettingScope() const override
	{
		return LocalPlayer;
	}
	// End of IEnhancedSettingsContainerAccessor interface

	UFUNCTION(BlueprintCallable)
	void CallOnLoadedEvents() const
	{
		EnhancedSettingsContainer->CallOnLoadedEvents();
	}

	UFUNCTION(BlueprintCallable)
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SaveSettings() const
	{
		GEngine->GetGameUserSettings()->SaveSettings();
		GetEnhancedSettingsContainer()->SaveSettings();
	}

	UFUNCTION(BlueprintCallable)
	void LoadSettings() const
	{
		GEngine->GetGameUserSettings()->LoadSettings();
		GEngine->GetGameUserSettings()->ApplySettings(false);
		GetEnhancedSettingsContainer()->LoadSettings();
	}
};
