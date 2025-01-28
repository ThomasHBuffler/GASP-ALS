// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnhancedSettingDelegateBinding.h"

#include "EnhancedSettingsEngineSubsystem.h"
#include "EnhancedSettingsGameInstanceSubsystem.h"
#include "EnhancedSettingsLocalPlayerSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnhancedSettingDelegateBinding)

TSet<UClass*> USettingDelegateBinding::SettingBindingClasses;

USettingDelegateBinding::USettingDelegateBinding(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (IsTemplate())
	{
		// Auto register the class
		SettingBindingClasses.Emplace(GetClass());
	}
}

bool USettingDelegateBinding::SupportsSettingDelegate(const UClass* InClass)
{
	return !!Cast<UBlueprintGeneratedClass>(InClass);
}

void USettingDelegateBinding::BindLocalPlayerEnhancedSettingWithSubobjects(AActor* InActor, UEnhancedSettingsLocalPlayerSubsystem* SettingsLocalPlayerSubsystem)
{
	ensureMsgf(InActor && SettingsLocalPlayerSubsystem, TEXT("Attempting to bind Enhanced Setting delegates to an invalid actor or subsystem!"));

	const UClass* ActorClass = InActor ? InActor->GetClass() : nullptr;

	if (ActorClass && SettingsLocalPlayerSubsystem && SupportsSettingDelegate(ActorClass))
	{
		// Bind any input delegates on the base actor class
		BindLocalPlayerEnhancedSettingDelegates(ActorClass, SettingsLocalPlayerSubsystem, InActor); //InputComponent->GetOwner()

		// Bind any input delegates on the actor's components
		TInlineComponentArray<UActorComponent*> ComponentArray;
		InActor->GetComponents(ComponentArray);
		for(UActorComponent* Comp : ComponentArray)
		{
			if(const UClass* CompClass = Comp ? Comp->GetClass() : nullptr)
			{
				BindLocalPlayerEnhancedSettingDelegates(CompClass, SettingsLocalPlayerSubsystem, Comp);	
			}			
		}
	}
}

void USettingDelegateBinding::BindLocalPlayerEnhancedSettingDelegates(const UClass* InClass, UEnhancedSettingsLocalPlayerSubsystem* SettingsLocalPlayerSubsystem, UObject* ObjectToBindTo)
{
	if (InClass && SettingsLocalPlayerSubsystem)
	{
		ensureMsgf(SettingsLocalPlayerSubsystem, TEXT("Attempting to bind Setting delegates to an invalid setting subsystem!"));
		ensureMsgf(ObjectToBindTo, TEXT("Attempting to bind Setting delegates to an invalid object!"));
		// If there was an object given to bind to use that, otherwise fall back to the input component's owner
		// which will be an AActor.
		
		BindLocalPlayerEnhancedSettingDelegates(InClass->GetSuperClass(), SettingsLocalPlayerSubsystem, ObjectToBindTo);
		
		for(UClass* BindingClass : SettingBindingClasses)
		{
			USettingDelegateBinding* BindingObject = CastChecked<USettingDelegateBinding>(
				UBlueprintGeneratedClass::GetDynamicBindingObject(InClass, BindingClass)
				, ECastCheckedType::NullAllowed);
			
			if (BindingObject)
			{
				BindingObject->BindToLocalPlayerSubsystem(SettingsLocalPlayerSubsystem, ObjectToBindTo);
			}
		}
	}
}

void USettingDelegateBinding::BindEnhancedSettingWithSubobjects(AActor* InActor, USubsystem* Subsystem)
{
	ensureMsgf(InActor && Subsystem, TEXT("Attempting to bind Enhanced Setting delegates to an invalid actor or subsystem!"));

	const UClass* ActorClass = InActor ? InActor->GetClass() : nullptr;

	if (ActorClass && Subsystem && SupportsSettingDelegate(ActorClass))
	{
		// Bind any input delegates on the base actor class
		BindEnhancedSettingDelegates(ActorClass, Subsystem, InActor); //InputComponent->GetOwner()

		// Bind any input delegates on the actor's components
		TInlineComponentArray<UActorComponent*> ComponentArray;
		InActor->GetComponents(ComponentArray);
		for(UActorComponent* Comp : ComponentArray)
		{
			if(const UClass* CompClass = Comp ? Comp->GetClass() : nullptr)
			{
				BindEnhancedSettingDelegates(CompClass, Subsystem, Comp);	
			}			
		}
	}
}

void USettingDelegateBinding::BindEnhancedSettingDelegates(const UClass* InClass, USubsystem* Subsystem, UObject* ObjectToBindTo)
{
	if (!Subsystem) return;
	
	if (InClass)
	{		
		BindEnhancedSettingDelegates(InClass->GetSuperClass(), Subsystem, ObjectToBindTo);
		
		for(UClass* BindingClass : SettingBindingClasses)
		{
			USettingDelegateBinding* BindingObject = CastChecked<USettingDelegateBinding>(
				UBlueprintGeneratedClass::GetDynamicBindingObject(InClass, BindingClass)
				, ECastCheckedType::NullAllowed);
			
			if (BindingObject)
			{
				BindingObject->BindToSubsystem(Subsystem, ObjectToBindTo);
			}
		}
	}
}

UEnhancedSettingDelegateBinding::UEnhancedSettingDelegateBinding(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEnhancedSettingDelegateBinding::BindToSubsystem(USubsystem* Subsystem, UObject* ObjectToBindTo) const
{
	if (!Subsystem) return;
	
	for(const FBlueprintEnhancedSettingBinding& Binding : SettingDelegateBindings)
	{
		TryToBindToSubsystem(Subsystem, Binding, ObjectToBindTo);
	}
}

void UEnhancedSettingDelegateBinding::BindToLocalPlayerSubsystem(UEnhancedSettingsLocalPlayerSubsystem* SettingsLocalPlayerSubsystem, UObject* ObjectToBindTo) const
{
	if (!SettingsLocalPlayerSubsystem) return;
	
	for(const FBlueprintEnhancedSettingBinding& Binding : SettingDelegateBindings)
	{
		TryToBindToSubsystem(SettingsLocalPlayerSubsystem, Binding, ObjectToBindTo);
	}
}

void UEnhancedSettingDelegateBinding::TryToBindToSubsystem(USubsystem* Subsystem, const FBlueprintEnhancedSettingBinding& Binding, UObject* ObjectToBindTo)
{
	if (!Subsystem->IsA(Binding.SubsystemClassToBindTo)) return;
	IEnhancedSettingsContainerAccessor* ContainerAccessor = Cast<IEnhancedSettingsContainerAccessor>(Subsystem);
	
	checkf(ContainerAccessor, TEXT("The given object to bind to does not implement IEnhancedSettingsContainerAccessor!"));
	checkf(ObjectToBindTo, TEXT("The given object to bind to is invalid!"));
	checkf(Binding.SubsystemClassToBindTo, TEXT("The given class to bind to is invalid!"));
	
	ContainerAccessor->GetEnhancedSettingsContainer()->BindEnhancedSetting(const_cast<UEnhancedSetting*>(Binding.EnhancedSetting.Get()), Binding.TriggerEvent, ObjectToBindTo, Binding.FunctionNameToBind);
}