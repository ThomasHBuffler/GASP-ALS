// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "NodeMetadata.generated.h"

class UEnhancedSetting;
enum class EEnhancedSettingValueType : uint8;

constexpr FLinearColor NodeDefaultColor = FLinearColor(0.24f, 0.0f, 1.0f); // Example color (blueish)

struct FValueTypeData
{
	FValueTypeData(FName InCategory, FName InSubCategory = NAME_None, UScriptStruct* InSubCategoryObject = nullptr)
		: Category(InCategory), SubCategory(InSubCategory), SubCategoryObject(InSubCategoryObject)
	{}

	FName Category;
	FName SubCategory;
	UScriptStruct* SubCategoryObject;
};

// Declare function signatures here
const TMap<EEnhancedSettingValueType, FValueTypeData>& GetSettingValueLookups();
FName GetValueCategory(const UEnhancedSetting* InSetting);
FName GetValueSubCategory(const UEnhancedSetting* InSetting);
UScriptStruct* GetValueSubCategoryObject(const UEnhancedSetting* InSetting);

FName GetEnhancedSettingName(const UEnhancedSetting* Setting);