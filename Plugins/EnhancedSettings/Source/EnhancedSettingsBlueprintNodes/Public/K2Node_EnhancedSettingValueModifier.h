// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnhancedSettingsTypes.h"
#include "K2Node_CallFunction.h"
#include "K2Node_EnhancedSettingValueModifier.generated.h"

class UEnhancedSetting;

UCLASS()
class ENHANCEDSETTINGSBLUEPRINTNODES_API UK2Node_EnhancedSettingValueModifier : public UK2Node_CallFunction
{
	GENERATED_BODY()

public:
	void Initialize(const UEnhancedSetting* InSetting);
	virtual void AllocateDefaultPins() override;

private:
	UPROPERTY()
	const UEnhancedSetting* Setting;
};