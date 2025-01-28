// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_EnhancedSettingValueModifier.h"
#include "EnhancedSettingsTypes.h"

#include "EnhancedSetting.h"
#include "EnhancedSettingsEngineSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_EnhancedSettingValueModifier)

void UK2Node_EnhancedSettingValueModifier::Initialize(const UEnhancedSetting* InSetting)
{
	Setting = InSetting;
	
	switch (InSetting->GetSettingValueType())
	{
	case EEnhancedSettingValueType::Boolean:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, ChangeBoundBoolValue)));
		break;
	case EEnhancedSettingValueType::Integer:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, ChangeBoundIntValue)));
		break;
	case EEnhancedSettingValueType::Float:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, ChangeBoundFloatValue)));
		break;
	case EEnhancedSettingValueType::Color:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, ChangeBoundColorValue)));
		break;
	case EEnhancedSettingValueType::Tag:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, ChangeBoundTagValue)));
		break;
		
	default:
		checkf(false, TEXT("Error unhandled setting type"))
	}
}

void UK2Node_EnhancedSettingValueModifier::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	UEdGraphPin* SettingPin = FindPinChecked(TEXT("Setting"));
	SettingPin->DefaultObject = const_cast<UEnhancedSetting*>(ToRawPtr(Setting));
}