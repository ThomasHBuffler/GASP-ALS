// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EnhancedSettingsTypes.h"
#include "EnhancedSettingsContainerAccessor.generated.h"

class UEnhancedSettingsContainer;

#pragma region Interface
UINTERFACE(Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingsContainerAccessor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for accessing UEnhancedSettingsContainer from a subsystem.
 */
class ENHANCEDSETTINGS_API IEnhancedSettingsContainerAccessor
{
	GENERATED_BODY()

public:
	virtual UEnhancedSettingsContainer* GetEnhancedSettingsContainer() const = 0;

	virtual EEnhancedSettingScope GetEnhancedSettingScope() const = 0;
};
#pragma endregion
