// Copyright (c) 2024 Thomas Hawke Buffler. All rights reserved.This software and its accompanying documentation are the proprietary property of [Your Full Name]. Unauthorized copying, distribution, modification, or use of this software, in whole or in part, is strictly prohibited without prior written permission from the copyright owner.For inquiries, contact: ThomasHBufflerAccess@gmail.com

#pragma once

#include "CoreMinimal.h"
#include "EnhancedSettingsContainer.h"
#include "EnhancedSettingsContainerAccessor.h"
#include "EnhancedSettingsTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnhancedSettingsGameInstanceSubsystem.generated.h"

class USettingsContainer;

UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingsGameInstanceSubsystem : public UGameInstanceSubsystem, public IEnhancedSettingsContainerAccessor
{
	GENERATED_BODY()

	UEnhancedSettingsGameInstanceSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

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
		return GameInstance;
	}
	// End of IEnhancedSettingsContainerAccessor interface

	UFUNCTION(BlueprintCallable)
	void CallOnLoadedEvents() const
	{
		EnhancedSettingsContainer->CallOnLoadedEvents();
	}
};
