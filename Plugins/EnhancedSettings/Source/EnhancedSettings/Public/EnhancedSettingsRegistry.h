// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedSetting.h"
#include "InputMappingContext.h"
#include "Engine/DataAsset.h"
#include "EnhancedSettingsRegistry.generated.h"

/**
 * A list of settings that can be grouped together.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingGroup : public UObject
{
	GENERATED_BODY()

public:
	// A localized name of this group for user interface
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|UserFacing")
	FText CategoryName = FText::GetEmpty();
	
	// A localized descriptor of this group
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|UserFacing", meta = (MultiLine))
	FText CategoryDescription = FText::GetEmpty();

	// Should this group be displayed in an advanced view?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|UserFacing")
	bool bAdvancedView = false;
};

/**
 * A list of settings grouped by a mapping context.
 */
UCLASS(Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingGroupSettings : public UEnhancedSettingGroup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UEnhancedSettingBase>> Settings;
};

/**
 * A mapping context (Grouped input actions).
 */
UCLASS(Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingGroupMappingContext : public UEnhancedSettingGroup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> Context;
};

/**
 * A mapping context (Grouped input actions).
 */
UCLASS(Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingWidget : public UEnhancedSettingGroup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> Widget;
};

/**
 * A 'Category' representing a collection of settings.
 *
 * Usually Grouped by a common theme, Such as (System: Graphics, Display, Audio, etc.)
 */
UCLASS(DefaultToInstanced, EditInlineNew, BlueprintType, Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingsCategory : public UObject
{
	GENERATED_BODY()

public:
	// A localized name of this category for user interface
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|UserFacing")
	FText CategoryName = FText::GetEmpty();
	
	// A localized descriptor of this category
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|UserFacing", meta = (MultiLine))
	FText CategoryDescription = FText::GetEmpty();

	// Should this category be displayed in an advanced view?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAdvancedView = false;
	
	// A localized name of this setting for user interface
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Setting|UserFacing")
	TArray<TObjectPtr<UEnhancedSettingGroup>> SettingGroups;
};

/**
 * A 'Tab' representing a collection of categorized settings and sub-tabs
 *
 * Stores all related categories and sub-tabs. (System, Gameplay, Controls, etc.)
 */
UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingsTab : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// A localized name of this setting for user interface
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|Description")
	FText TabName = FText::GetEmpty();
	
	// A localized descriptor of this setting
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting|Description", meta = (MultiLine))
	FText TabDescription = FText::GetEmpty();

	// The individual categories that make up this tab
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "CustomObjects")
	TArray<UEnhancedSettingsCategory*> Categories;
};

/**
 * A 'Sub-Category' representing a collection of settings.
 * 
 * This is like a mini tab it can be used to further categorize settings within a category and have an advanced settings view.
 */