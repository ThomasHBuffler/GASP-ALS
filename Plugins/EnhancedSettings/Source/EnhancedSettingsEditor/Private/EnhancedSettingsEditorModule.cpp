// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnhancedSettingsEditorModule.h"

#include "AssetBlueprintGraphActions.h"
#include "BlueprintEditorModule.h"
#include "BlueprintNodeTemplateCache.h"
#include "Brushes/SlateImageBrush.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Misc/PackageName.h"
#include "EnhancedSettingCustomizations.h"
#include "ISettingsModule.h"
#include "ToolMenuSection.h"
#include "PropertyEditorModule.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "AssetTypeActions/AssetTypeActions_DataAsset.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "ContentBrowserModule.h"
#include "EnhancedSettingsRegistry.h"
#include "EdGraphSchema_K2_Actions.h"
#include "IContentBrowserSingleton.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Kismet2/SClassPickerDialog.h"
#include "GameFramework/InputSettings.h"
#include "Interfaces/IMainFrameModule.h"
#include "SourceControlHelpers.h"
#include "HAL/FileManager.h"
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "EnhancedSetting.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnhancedSettingsEditorModule)

#define LOCTEXT_NAMESPACE "EnhancedSettings"

DEFINE_LOG_CATEGORY(LogEnhancedSettingsEditor);

EAssetTypeCategories::Type FEnhancedSettingsEditorModule::EnhancedSettingAssetsCategory;

IMPLEMENT_MODULE(FEnhancedSettingsEditorModule, EnhancedSettingsEditor)

class FEnhancedSettingClassParentFilter : public IClassViewerFilter
{
public:
	FEnhancedSettingClassParentFilter()
		: DisallowedClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown) {}

	/** All children of these classes will be included unless filtered out by another setting. */
	TSet<const UClass*> AllowedChildrenOfClasses;

	/** Disallowed class flags. */
	EClassFlags DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};

const UEnhancedSetting* FEnhancedSettingsEditorModule::FindEnhancedSettingByTag(const FGameplayTag InID)
{
	// Instanced objects that have been deleted will have an outer of the transient package, and we don't want to include them
	for (TObjectIterator<UEnhancedSetting> Itr(RF_ClassDefaultObject | RF_Transient, true, EInternalObjectFlags::Garbage | UE::GC::GUnreachableObjectFlag); Itr; ++Itr)
	{
		const UEnhancedSetting* Settings = *Itr;
		if (Settings && Settings->GetOuter() != GetTransientPackage() && Settings->GetUniqueSettingID() == InID)
		{
			// What if we had just removed a mapping of this name?
			return Settings;
		}
	}
	return nullptr;
}

bool FEnhancedSettingsEditorModule::IsEnhancedSettingIDTagUsed(const FGameplayTag InID)
{
	// If a mapping with this name exists, then it is in use
	return FindEnhancedSettingByTag(InID) != nullptr;
}

// Asset factories

UEnhancedSetting_Factory::UEnhancedSetting_Factory(const class FObjectInitializer& OBJ)
	: Super(OBJ)
{
	SupportedClass = UEnhancedSetting::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

bool UEnhancedSetting_Factory::ConfigureProperties()
{	
	// nullptr the EnhancedSettingClass so we can check for selection
	EnhancedSettingClass = nullptr;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	TSharedPtr<FEnhancedSettingClassParentFilter> Filter = MakeShareable(new FEnhancedSettingClassParentFilter);
	Filter->AllowedChildrenOfClasses.Add(UEnhancedSetting::StaticClass());

	Options.ClassFilters.Add(Filter.ToSharedRef());

	const FText TitleText = LOCTEXT("CreateEnhancedSettingOptions", "Pick Class For Enhanced Setting Instance");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UEnhancedSetting::StaticClass());

	if (bPressedOk)
	{
		EnhancedSettingClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UEnhancedSetting_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{	
	if (EnhancedSettingClass != nullptr)
	{
		return NewObject<UEnhancedSetting>(InParent, EnhancedSettingClass, Name, Flags | RF_Transactional, Context);
	}
	else
	{
		check(Class->IsChildOf(UEnhancedSetting::StaticClass()));
		return NewObject<UEnhancedSetting>(InParent, Class, Name, Flags | RF_Transactional, Context);
	}
}

class FAssetTypeActions_EnhancedSetting : public FAssetTypeActions_DataAsset
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EnhancedSetting", "Enhanced Setting"); }
	virtual uint32 GetCategories() override { return FEnhancedSettingsEditorModule::GetEnhancedSettingAssetsCategory(); }
	virtual FColor GetTypeColor() const override { return FColor(110, 0, 225); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EnhancedSettingDesc", "Represents a definition for a setting for the game."); }
	virtual UClass* GetSupportedClass() const override { return UEnhancedSetting::StaticClass(); }

};

class FAssetTypeActions_EnhancedSettingsTab : public FAssetTypeActions_DataAsset
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EnhancedSettingsTab", "Enhanced Settings Tab"); }
	virtual uint32 GetCategories() override { return FEnhancedSettingsEditorModule::GetEnhancedSettingAssetsCategory(); }
	virtual FColor GetTypeColor() const override { return FColor(0, 255, 185); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EnhancedSettingDesc", "A collection of settings and sub-tabs, meant to represent settings in the UI"); }
	virtual UClass* GetSupportedClass() const override { return UEnhancedSettingsTab::StaticClass(); }

};

/** Custom style set for Enhanced Input */
class FEnhancedSettingSlateStyle final : public FSlateStyleSet
{
public:
	FEnhancedSettingSlateStyle()
		: FSlateStyleSet("EnhancedSettingEditor")
	{
		
		SetParentStyleName(FAppStyle::GetAppStyleSetName());

		// The icons are located in /Project/Plugins/EnhancedSettings/Content/Editor/Slate/Icons
		SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("EnhancedSettings/Content/Editor/Slate"));
		SetCoreContentRoot(FPaths::ProjectPluginsDir() / TEXT("Slate"));

		// Enhanced Input Editor icons
		static const FVector2D Icon16 = FVector2D(16.0f, 16.0f);
		static const FVector2D Icon64 = FVector2D(64.0f, 64.0f);

		Set("ClassIcon.EnhancedSetting", new IMAGE_BRUSH_SVG("Icons/EnhancedSetting_16", Icon16));
		Set("ClassThumbnail.EnhancedSetting", new IMAGE_BRUSH_SVG("Icons/EnhancedSetting_64", Icon64));
	}
};

void FEnhancedSettingsEditorModule::StartupModule()
{
	/*
	// Register customizations
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout(UEnhancedSetting::StaticClass()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEnhancedSettingCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("EnhancedSetting", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEnhancedSettingCustomization::MakeInstance));
	
	PropertyModule.NotifyCustomizationModuleChanged();

	// Register Setting assets
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EnhancedSettingAssetsCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("EnhancedSetting")), LOCTEXT("EnhancedSettingAssetsCategory", "Enhanced Setting"));
	{
		RegisterAssetTypeActions(AssetTools, MakeShareable(new FAssetTypeActions_EnhancedSetting));
		RegisterAssetTypeActions(AssetTools, MakeShareable(new FAssetTypeActions_EnhancedSettingsTab));
	}
	
	
	// Make a new style set for Enhanced Input, which will register any custom icons for the types in this plugin
	StyleSet = MakeShared<FEnhancedSettingSlateStyle>();
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
	*/
}

void FEnhancedSettingsEditorModule::ShutdownModule()
{
	
	// Unregister input assets
	if (FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		for (TSharedPtr<IAssetTypeActions>& AssetAction : CreatedAssetTypeActions)
		{
			AssetToolsModule->Get().UnregisterAssetTypeActions(AssetAction.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();

	// Unregister input settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Engine", "Enhanced Input");
	}

	// Unregister customizations
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// Unregister slate stylings
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	}
	/**/
}

EAssetTypeCategories::Type FEnhancedSettingsEditorModule::GetEnhancedSettingAssetsCategory()
{
	return EnhancedSettingAssetsCategory;
}

void FEnhancedSettingsEditorModule::RegisterAssetTypeActions(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE