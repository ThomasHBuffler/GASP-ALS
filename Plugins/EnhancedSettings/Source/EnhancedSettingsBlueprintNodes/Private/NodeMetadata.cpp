// Fill out your copyright notice in the Description page of Project Settings.


#include "NodeMetadata.h"
#include "EdGraphSchema_K2.h"
#include "EnhancedSetting.h"
#include "EnhancedSettingsTypes.h"
#include "GameplayTagContainer.h"

// Implement functions here
const TMap<EEnhancedSettingValueType, FValueTypeData>& GetSettingValueLookups()
{
	static const TMap<EEnhancedSettingValueType, FValueTypeData> ValueLookups =
	{
		{ EEnhancedSettingValueType::NONE, FValueTypeData(UEdGraphSchema_K2::PC_Boolean) },
		{ EEnhancedSettingValueType::Boolean, FValueTypeData(UEdGraphSchema_K2::PC_Boolean) },
		{ EEnhancedSettingValueType::Integer, FValueTypeData(UEdGraphSchema_K2::PC_Int) },
		{ EEnhancedSettingValueType::Float, FValueTypeData(UEdGraphSchema_K2::PC_Real, UEdGraphSchema_K2::PC_Float) },
		{ EEnhancedSettingValueType::Color, FValueTypeData(UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FColor>::Get()) },
		{ EEnhancedSettingValueType::Tag, FValueTypeData(UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FGameplayTag>::Get()) },
	};
	return ValueLookups;
}

FName GetValueCategory(const UEnhancedSetting* InSetting)
{
	if (!IsValid(InSetting)) return FName("NONE");
	EEnhancedSettingValueType Type = InSetting->GetSettingValueType();
	return GetSettingValueLookups()[Type].Category;
}

FName GetValueSubCategory(const UEnhancedSetting* InSetting)
{
	if (!IsValid(InSetting)) return FName("NONE");
	EEnhancedSettingValueType Type = InSetting->GetSettingValueType();
	return GetSettingValueLookups()[Type].SubCategory;
}

UScriptStruct* GetValueSubCategoryObject(const UEnhancedSetting* InSetting)
{
	if (!IsValid(InSetting)) return nullptr;
	EEnhancedSettingValueType Type = InSetting->GetSettingValueType();
	return GetSettingValueLookups()[Type].SubCategoryObject;
}

FName GetEnhancedSettingName(const UEnhancedSetting* Setting)
{
	return  Setting ? Setting->GetFName() : FName();
}
