// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "Modules/ModuleInterface.h"
#include "AssetTypeCategories.h"

#include "EnhancedSettingsEditorModule.generated.h"

struct FGameplayTag;
class UEnhancedSetting;
DECLARE_LOG_CATEGORY_EXTERN(LogEnhancedSettingsEditor, Log, All);

////////////////////////////////////////////////////////////////////
// FInputEditorModule

class UInputAction;
class SWindow;
class UPlayerMappableKeySettings;
class IAssetTypeActions;
class IAssetTools;
class FSlateStyleSet;

class FEnhancedSettingsEditorModule : public IModuleInterface
{
public:

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModuleInterface interface

	static EAssetTypeCategories::Type GetEnhancedSettingAssetsCategory();
	
	/** Returns a pointer to the player Enhanced Setting object that has this mapping name */
	ENHANCEDSETTINGSEDITOR_API static const UEnhancedSetting* FindEnhancedSettingByTag(const FGameplayTag InID);
	
	/** Returns true if the given name is in use by a player mappable key setting */
	ENHANCEDSETTINGSEDITOR_API static bool IsEnhancedSettingIDTagUsed(const FGameplayTag InID);
private:
	void RegisterAssetTypeActions(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

	static EAssetTypeCategories::Type EnhancedSettingAssetsCategory;
	
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
	
	TSharedPtr<FSlateStyleSet> StyleSet;
};

////////////////////////////////////////////////////////////////////
// Asset factories

UCLASS()
class ENHANCEDSETTINGSEDITOR_API UEnhancedSetting_Factory : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category=EnhancedSetting)
	TSubclassOf<UEnhancedSetting> EnhancedSettingClass;

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#include "CoreMinimal.h"
#include "IDetailsView.h"
#endif
