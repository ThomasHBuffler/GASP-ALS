// Copyright Epic Games, Inc. All Rights Reserved.

#include "K2Node_EnhancedSettingEvent.h"
#include "EnhancedSettingDelegateBinding.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_EnhancedSettingEvent)

UK2Node_EnhancedSettingEvent::UK2Node_EnhancedSettingEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), SubsystemClass(nullptr)
{
	bInternalEvent = true;
	TriggerEvent = EEnhancedSettingChangeEvent::None;
}

UClass* UK2Node_EnhancedSettingEvent::GetDynamicBindingClass() const
{
	return UEnhancedSettingDelegateBinding::StaticClass();
}

void UK2Node_EnhancedSettingEvent::RegisterDynamicBinding(UDynamicBlueprintBinding* BindingObject) const
{
	UEnhancedSettingDelegateBinding* SettingBindingObject = CastChecked<UEnhancedSettingDelegateBinding>(BindingObject);

	FBlueprintEnhancedSettingBinding Binding;
	Binding.EnhancedSetting = EnhancedSetting;
	Binding.TriggerEvent = TriggerEvent;
	Binding.FunctionNameToBind = CustomFunctionName;
	Binding.SubsystemClassToBindTo = SubsystemClass;
	
	SettingBindingObject->SettingDelegateBindings.Add(Binding);
}