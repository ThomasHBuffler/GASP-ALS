// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"

struct FSmartObjectEventData;
class UEnhancedSetting;
class FKeyStructCustomization;
class UPlayerMappableKeySettings;
class IDetailPropertyRow;
class IDetailLayoutBuilder;

#define GET_GAMEPLAY_TAG_NAME_CHECKED(ClassName, MemberName) \
((void)sizeof(UEAsserts_Private::GetMemberNameCheckedJunk(((ClassName*)0)->MemberName)), \
FGameplayTag::RequestGameplayTag(TEXT(#MemberName)))

/**
 * Input customization for UPlayerMappableKeySettings to have some name validation
 * on the custom saved FNames for mappings.
 */
class FEnhancedSettingCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
	return MakeShareable(new FEnhancedSettingCustomization());
	}
	
	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	
	UEnhancedSetting* GetSettingsObject() const;
	void OnTagChanged();
	
	FGameplayTag GetGameplayTag() const;
	//bool OnVerifyMappingName(const FText& InNewText, FText& OutErrorMessage);

	TSharedPtr<IPropertyHandle> SettingsPropHandle;
	TSharedPtr<IPropertyHandle> MappingNamePropHandle;

	void OnSmartDataUpdated (FGameplayTag Tag, int32 Index);
};
