// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnhancedSettingCustomizations.h"

#include "AssetRegistry/ARFilter.h"
#include "DetailLayoutBuilder.h"
#include "EnhancedSettingsEditorModule.h"
#include "Engine/Blueprint.h"
#include "IDetailChildrenBuilder.h"
#include "KeyStructCustomization.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/BlueprintSupport.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectIterator.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "EnhancedSettingsTagIdValidator.h"
#include "SGameplayTagWidget.h"
#include "EnhancedSetting.h"

#define LOCTEXT_NAMESPACE "EnhancedSettingCustomization"


void FEnhancedSettingCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	SettingsPropHandle = PropertyHandle;
	UEnhancedSetting* Settings = GetSettingsObject();
	
}

void FEnhancedSettingCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UE_LOG(LogEnhancedSettingsEditor, Error, TEXT("CustomizeChildren"));

	/*
	// Customize the FGameplayTag handle to have some validation on what users can enter into it
	static const FName MappingPropertyHandleName = GET_MEMBER_NAME_CHECKED(UEnhancedSetting, UniqueSettingID);
	
	uint32 NumChildren = 0;
	TSharedPtr<IPropertyHandle> SettingsHandle;
	TSharedPtr<IPropertyHandle> TempHandle = PropertyHandle->GetChildHandle(0);

	if (TempHandle.IsValid())
	{
		TempHandle->GetNumChildren(NumChildren);

		// For instanced properties there can be multiple layers of "children" until we get to the ones
		// that we care about. Iterate all the children until we find one with more then one
		// child, or we run out of children.
		while (!SettingsHandle.IsValid() || !TempHandle.IsValid())
		{
			TempHandle->GetNumChildren(NumChildren);

			if (NumChildren > 1)
			{
				SettingsHandle = TempHandle;
			}
			else
			{
				TempHandle = TempHandle->GetChildHandle(0);
			}		
		}	
	}	
	
	if (!SettingsHandle.IsValid())
	{
		return;
	}

	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		TSharedPtr<IPropertyHandle> ChildHandle = SettingsHandle->GetChildHandle(ChildIndex);
		if (!ChildHandle.IsValid())
		{
			continue;
		}
		
		const FName PropertyName = ChildHandle->GetProperty()->GetFName();

		// Add a customization to handle name validation on the mappable name if it's enabled
		if (PropertyName == MappingPropertyHandleName)
		{
			MappingNamePropHandle = ChildHandle;
			IDetailPropertyRow& NameRow = ChildBuilder.AddProperty(MappingNamePropHandle.ToSharedRef());       
			
		
			TSharedRef<SGameplayTagWidget> GTW = 
				MakeShared<SGameplayTagWidget>();
			
			// Prepare the arguments required by Construct().
			SGameplayTagWidget::FArguments Args = SGameplayTagWidget::FArguments()
			.GameplayTagUIMode(EGameplayTagUIMode::SelectionMode)  // Use selection mode
			.ReadOnly(false)  // If you want the widget to be read-only or editable
			.MultiSelect(false)  // Allow multiple tag selections
			//.RestrictedTags(false)  // If restricted tags should be shown
			.TagContainerName(TEXT("Unique Setting Identification Tag"))  // Name for this tag container (for saving state)
			//.PropertyHandle(PropertyHandle)  // Optional: If linked to a property handle
			.OnTagChanged(this, &FEnhancedSettingCustomization::OnTagChanged); // Delegate when a ta

			FGameplayTagContainer* TagContainer = new FGameplayTagContainer();
			TArray<FGameplayTagContainer*> MyTagContainers = {TagContainer};
			
			// Use the relevant owner UObject

			TArray<SGameplayTagWidget::FEditableGameplayTagContainerDatum> EditableTagContainers;
			EditableTagContainers.Add(SGameplayTagWidget::FEditableGameplayTagContainerDatum(nullptr, TagContainer));
			
			// Now call Construct manually with the necessary parameters.
			GTW->Construct(Args, EditableTagContainers);

			
            NameRow.CustomWidget()
            .NameContent()
            [
            	MappingNamePropHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            [
				GTW
            ];
			UE_LOG(LogEnhancedSettingsEditor, Error, TEXT("OnCustomWidgetCreated"));
		}
		else
		{
			ChildBuilder.AddProperty(ChildHandle.ToSharedRef());
		}
	}
	*/
}

UEnhancedSetting* FEnhancedSettingCustomization::GetSettingsObject() const
{
	if (SettingsPropHandle.IsValid())
	{
		UObject* SettingsObject = nullptr;
		SettingsPropHandle->GetValue(SettingsObject);
		return Cast<UEnhancedSetting>(SettingsObject);	
	}

	return nullptr;
}

void FEnhancedSettingCustomization::OnTagChanged()
{
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Orange, "ON TAG CHANGED!");
	UE_LOG(LogEnhancedSettingsEditor, Error, TEXT("ON TAG CHANGED!"));
}

FGameplayTag FEnhancedSettingCustomization::GetGameplayTag() const
{
	// this is the value in the actual text box
	if (UEnhancedSetting* Settings = GetSettingsObject())
	{
		return Settings->GetUniqueSettingID();
	}
	
	return FGameplayTag();
}
#undef LOCTEXT_NAMESPACE
