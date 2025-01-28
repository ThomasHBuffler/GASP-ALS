// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnhancedSettingsTagIdValidator.h"
#include "EnhancedSettingsEditorModule.h"
#include "GameplayTagContainer.h"
#include "Internationalization/Text.h"	// For FFormatNamedArguments
#include "HAL/IConsoleManager.h"		// For FAutoConsoleVariableRef

// Example implementation of IsTagValid
bool FGameplayTagValidator::IsTagValid(const FGameplayTag& Tag)
{
	// Add your validation logic here.
	// For example, check if the tag is valid and not empty.
	return Tag.IsValid();
}

// Example implementation of AreTagsValid
bool FGameplayTagValidator::AreTagsValid(const FGameplayTagContainer& Tags)
{
	// Iterate through the container and validate each tag
	for (const FGameplayTag& Tag : Tags)
	{
		if (!IsTagValid(Tag))
		{
			return false; // Return false if any tag is invalid
		}
	}
	return true; // All tags are valid
}

// Example implementation of GetValidationErrorMessage
FString FGameplayTagValidator::GetValidationErrorMessage()
{
	return TEXT("Some gameplay tags are invalid."); // Customize this message as needed
}

/*
FEnhancedSettingsTagIdValidator::FEnhancedSettingsTagIdValidator(FGameplayTag InExistingTag)
	: FStringSetNameValidator(InExistingName.ToString())
{ }

EValidatorResult FEnhancedSettingsTagIdValidator::IsValid(const FString& Name, bool bOriginal)
{
	// Ensure that the length of this name is a valid length
	if (Name.Len() >= UE::EnhancedInput::GetMaxAcceptableLength())
	{
		return EValidatorResult::TooLong;
	}
	
	EValidatorResult Result = FStringSetNameValidator::IsValid(Name, bOriginal);

	if (UE::EnhancedInput::bEnableMappingNameValidation &&
		Result != EValidatorResult::ExistingName &&
		FEnhancedSettingsEditorModule::IsMappingNameInUse(FName(Name)))
	{
		Result = EValidatorResult::AlreadyInUse;
	}

	return Result;
}

FText FEnhancedSettingsTagIdValidator::GetErrorText(const FString& Name, EValidatorResult ErrorCode)
{
	// Attempt to specify what asset is using this name
	if (ErrorCode == EValidatorResult::AlreadyInUse)
	{
		if (const UPlayerMappableKeySettings* Settings = FInputEditorModule::FindMappingByName(FName(Name)))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("AssetUsingName"), FText::FromString(GetNameSafe(Settings->GetOuter())));

			return FText::Format(NSLOCTEXT("EnhancedInput", "MappingNameInUseBy_Error", "Name is already in use by '{AssetUsingName}'"), Args);	
		}
	}
	else if (ErrorCode == EValidatorResult::TooLong)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("MaxLength"), UE::EnhancedInput::GetMaxAcceptableLength());

		return FText::Format(NSLOCTEXT("EnhancedInput", "MappingNameTooLong_Error", "Names must be fewer then '{MaxLength}' characters"), Args);
	}
	
	return INameValidatorInterface::GetErrorText(Name, ErrorCode);
}
*/