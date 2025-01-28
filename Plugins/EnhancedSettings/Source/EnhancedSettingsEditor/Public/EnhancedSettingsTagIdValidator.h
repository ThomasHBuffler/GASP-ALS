// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

struct FGameplayTagContainer;
struct FGameplayTag;

struct FGameplayTagValidator
{
	// Validate if a specific gameplay tag is valid
	static bool IsTagValid(const FGameplayTag& Tag);

	// Validate a list of gameplay tags
	static bool AreTagsValid(const FGameplayTagContainer& Tags);

	// Optionally, provide a method to get a validation error message
	static FString GetValidationErrorMessage();
};