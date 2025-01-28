// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTags.h"
#include "EnhancedSettingsTypes.generated.h"

class EnhancedSetting;

#pragma region Scope
// The scope of the settings, where what subsystem controls them and in what context they will be available for use
UENUM()
enum EEnhancedSettingScope
{
	GameInstance,
	LocalPlayer,
};

inline const TMap<EEnhancedSettingScope, UClass*>& GetScopeSubsystemClassMap()
{
	static const TMap<EEnhancedSettingScope, UClass*> ValueLookups =
	{
		//{ Engine, UEngineSubsystem::StaticClass() },
		{ GameInstance, UGameInstanceSubsystem::StaticClass() },
		//{ World, UWorldSubsystem::StaticClass() },
		{ LocalPlayer, ULocalPlayerSubsystem::StaticClass() }
	};
	return ValueLookups;
}
#pragma endregion 

#pragma region SettingTypes(BPInterface)
UENUM(BlueprintType)
enum class EEnhancedSettingValueType : uint8
{
	NONE UMETA(Hidden),
	Boolean,
	Integer,
	Float,
	Int2D, // Currently only used for resolution settings, (GameUserSettings)
	Color,
	Tag,
};

USTRUCT(BlueprintType)
struct FEnhancedSettingValue
{
	GENERATED_BODY()
	
	EEnhancedSettingValueType ValueType = EEnhancedSettingValueType::NONE;

	UPROPERTY()
	bool BooleanValue = false;
	
	UPROPERTY()
	int IntegerValue = -1;
	
	UPROPERTY()
	float FloatValue = -1;
	
	UPROPERTY()
	FColor ColorValue = FColor::White;
	
	UPROPERTY()
	FGameplayTag TagValue = FGameplayTag();

	FEnhancedSettingValue() :
	ValueType(EEnhancedSettingValueType::NONE),
	BooleanValue(false),
	IntegerValue(-1),
	FloatValue(-1.0f),
	ColorValue(FColor()),
	TagValue(FGameplayTag()) {}

	FEnhancedSettingValue(bool InValue)
	{
		ValueType = EEnhancedSettingValueType::Boolean;
		BooleanValue = InValue;
	}
	FEnhancedSettingValue(int InValue)
	{
		ValueType = EEnhancedSettingValueType::Integer;
		IntegerValue = InValue;
	}
	FEnhancedSettingValue(float InValue)
	{
		ValueType = EEnhancedSettingValueType::Float;
		FloatValue = InValue;
	}
	FEnhancedSettingValue(FColor InValue)
	{
		ValueType = EEnhancedSettingValueType::Color;
		ColorValue = InValue;
	}
	FEnhancedSettingValue(FGameplayTag InValue)
	{
		ValueType = EEnhancedSettingValueType::Tag;
		TagValue = InValue;
	}
};
#pragma endregion

#pragma region EnhancedSettingValueContainters
// Template-based profile structure.
template <typename T>
struct TEnhancedSettingValues
{
	TMap<FGameplayTag, T> Values;
};

// Central container for settings of all types.
struct FEnhancedSettingValueContainer
{
	TEnhancedSettingValues<bool> BoolValues;
	TEnhancedSettingValues<int32> IntegerValues;
	TEnhancedSettingValues<float> FloatValues;
	TEnhancedSettingValues<FColor> ColorValues;
	TEnhancedSettingValues<FGameplayTag> TagValues;
	
	auto GetProfilesAsTuple() const -> std::tuple<
		const TEnhancedSettingValues<bool>&,
		const TEnhancedSettingValues<int32>&,
		const TEnhancedSettingValues<float>&,
		const TEnhancedSettingValues<FColor>&,
		const TEnhancedSettingValues<FGameplayTag>&>
	{
		return {BoolValues, IntegerValues, FloatValues, ColorValues, TagValues};
	}
	
	template <typename T>
	T GetBoundValue(const TEnhancedSettingValues<T>& Profile, FGameplayTag Key, const T& DefaultValue = T()) const
	{
		if (const T* Value = Profile.Values.Find(Key))
		{
			return *Value;
		}
		
		return DefaultValue;
	}
};
#pragma endregion

#pragma region ChangeEvents
/**
* EnhancedSetting events are the events that trigger once changes and commits are made to the setting
*/
UENUM(BlueprintType) //, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EEnhancedSettingChangeEvent : uint8
{
	// No trigger occured (No change)
	None UMETA(Hidden),//		= (0x0)	
	
	// Trigger once a setting is changed
	Changed, //	= (1 << 0),	

	// Trigger once a setting is committed (Changed -> Commited)
	Commited, // 		= (1 << 1),
	
	// Trigger once a setting is loaded (Current -> Loaded)
	Loaded, //	= (1 << 2),

	// Trigger once a setting change is undone (Changed -> Undo)
	Cleared, //		= (1 << 3),
	
	// Trigger once a setting is reset to default (Current -> Default)
	Reset, //		= (1 << 4),

	// Trigger once a setting was updated via stack change (Current -> NewStackValue)
	StackChanged, //		= (1 << 5),

	MAX UMETA(Hidden)
};
#pragma endregion