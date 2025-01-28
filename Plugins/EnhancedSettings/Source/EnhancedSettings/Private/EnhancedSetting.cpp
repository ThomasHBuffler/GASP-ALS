// Fill out your copyright notice in the Description page of Project Settings.


#include "EnhancedSetting.h"

#include "GameFramework/GameStateBase.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

const FPrimaryAssetType UEnhancedSetting::SettingPrimaryAssetType = TEXT("EnhancedSetting");

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnhancedSetting)

bool UESER_MainPlayer::IsRequirementMet_Implementation(APlayerController* PlayerController) const
{
	return PlayerController->GetWorld()->GetGameState()->PlayerArray[0] == PlayerController->PlayerState;
}

bool UEnhancedSettingBase::AreRequirementsMet(APlayerController* PlayerController) const
{
	for (auto& Requirement : Requirements)
	{
		if (!Requirement->IsRequirementMet(PlayerController)) return false;
	}
	return true;
}

#if WITH_EDITOR
EDataValidationResult UEnhancedSetting::IsDataValid(class FDataValidationContext& Context) const
{
	return Super::IsDataValid(Context);
}

void UEnhancedSettingInt::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UEnhancedSettingInt, MinValue))
	{
		if (MinValue > MaxValue) MaxValue = MinValue;
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UEnhancedSettingInt, MaxValue))
	{
		if (MaxValue < MinValue) MinValue = MaxValue;
	}
	if (bHasMinValue) Value = FMath::Max(Value, MinValue);
	if (bHasMaxValue) Value = FMath::Min(Value, MaxValue);
}

void UEnhancedSettingFloat::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UEnhancedSettingFloat, MinValue))
	{
		if (MinValue > MaxValue) MaxValue = MinValue;
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UEnhancedSettingFloat, MaxValue))
	{
		if (MaxValue < MinValue) MinValue = MaxValue;
	}
	if (bHasMinValue) Value = FMath::Max(Value, MinValue);
	if (bHasMaxValue) Value = FMath::Min(Value, MaxValue);
}
#endif