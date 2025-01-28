// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_EnhancedSettingValueAccessor.h"
#include "EnhancedSettingsTypes.h"
#include "EnhancedSetting.h"
#include "EnhancedSettingsEngineSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_EnhancedSettingValueAccessor)

void UK2Node_EnhancedSettingValueAccessor::Initialize(const UEnhancedSetting* InSetting)
{
	Setting = InSetting;
	
	switch (InSetting->GetSettingValueType())
	{
	case EEnhancedSettingValueType::Boolean:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, GetBoundBoolValue)));
		break;
	case EEnhancedSettingValueType::Integer:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, GetBoundIntValue)));
		break;
	case EEnhancedSettingValueType::Float:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, GetBoundFloatValue)));
		break;
	case EEnhancedSettingValueType::Color:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, GetBoundColorValue)));
		break;
	case EEnhancedSettingValueType::Tag:
		SetFromFunction(UEnhancedSettingsEngineSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UEnhancedSettingsEngineSubsystem, GetBoundTagValue)));
		break;
	default:
		checkf(false, TEXT("Error unhandled setting type"))
	}
}

void UK2Node_EnhancedSettingValueAccessor::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	UEdGraphPin* SettingPin = FindPinChecked(TEXT("Setting"));
	SettingPin->DefaultObject = const_cast<UEnhancedSetting*>(ToRawPtr(Setting));
}
