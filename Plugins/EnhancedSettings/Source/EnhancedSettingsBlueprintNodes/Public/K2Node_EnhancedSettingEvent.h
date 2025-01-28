// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "K2Node_Event.h"
#include "K2Node_EnhancedSettingEvent.generated.h"

enum class EEnhancedSettingChangeEvent : uint8;
class UEnhancedSetting;

class UDynamicBlueprintBinding;

UCLASS()
class ENHANCEDSETTINGSBLUEPRINTNODES_API UK2Node_EnhancedSettingEvent : public UK2Node_Event
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	TObjectPtr<const UEnhancedSetting> EnhancedSetting;

	UPROPERTY()
	EEnhancedSettingChangeEvent TriggerEvent;

	UPROPERTY()
	const UClass* SubsystemClass;

	//~ Begin UK2Node Interface
	virtual UClass* GetDynamicBindingClass() const override;
	virtual void RegisterDynamicBinding(UDynamicBlueprintBinding* BindingObject) const override;
	//~ End UK2Node Interface
};
