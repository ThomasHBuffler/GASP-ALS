// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EnhancedSettingsTypes.h"
#include "Engine/DynamicBlueprintBinding.h"
#include "EnhancedSettingDelegateBinding.generated.h"

class UEnhancedSettingsLocalPlayerSubsystem;
class UEnhancedSetting;
class USettingsLocalPlayerSubsystem;

//@TODO: Thomas review descriptions

USTRUCT()
struct ENHANCEDSETTINGS_API FBlueprintEnhancedSettingBinding
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TObjectPtr<const UEnhancedSetting> EnhancedSetting = nullptr;

	UPROPERTY()
	EEnhancedSettingChangeEvent TriggerEvent = EEnhancedSettingChangeEvent::None;

	UPROPERTY()
	FName FunctionNameToBind = NAME_None;

	UPROPERTY()
	const UClass* SubsystemClassToBindTo = nullptr;
};


UCLASS(abstract, MinimalAPI)
class USettingDelegateBinding : public UDynamicBlueprintBinding
{
	GENERATED_UCLASS_BODY()
	
	/**
	 * Override this function to bind a delegate to the given input component.
	 *
	 * @param SettingsLocalPlayerSubsystem		The SettingsLocalPlayerSubsystem to Bind a delegate to
	 * @param ObjectToBindTo		The UObject that the binding should use.
	 */
	virtual void BindToLocalPlayerSubsystem(UEnhancedSettingsLocalPlayerSubsystem* SettingsLocalPlayerSubsystem, UObject* ObjectToBindTo) const { };
	
	/**
		 * Override this function to bind a delegate to the given input component.
		 *
		 * @param ObjectToBindTo		The UObject that the binding should use.
		 */
	virtual void BindToSubsystem(USubsystem* Subsystem, UObject* ObjectToBindTo) const { };

	/** Returns true if the given class supports input binding delegates (i.e. it is a BP generated class) */
	static bool SupportsSettingDelegate(const UClass* InClass);

	/**
	 * Calls BindToSettingsLocalPlayerSubsystem for each dynamic binding object on the given Class if it supports input delegates.
	 * This is for non actor objects.
	 * 
	 * This is for PLAYER Specific settings.
	 *
	 * @param InClass				The class that will should be used to determine if Input Delegates are supported
	 * @param SettingsLocalPlayerSubsystem		The SettingsLocalPlayerSubsystem to Bind a delegate to
	 * @param ObjectToBindTo		The UObject that the binding should use. Should not be null
	 */
	UFUNCTION(BlueprintCallable)
	static void BindLocalPlayerEnhancedSettingDelegates(const UClass* InClass, UEnhancedSettingsLocalPlayerSubsystem* SettingsLocalPlayerSubsystem, UObject* ObjectToBindTo = nullptr);
	
	/**
	 * Will bind input delegates for the given Actor and traverse it's subobjects attempting to bind each of them
	 * 
	 * This is for PLAYER Specific settings.
	 */
	UFUNCTION(BlueprintCallable)
	static void BindLocalPlayerEnhancedSettingWithSubobjects(AActor* InActor, UEnhancedSettingsLocalPlayerSubsystem* SettingsLocalPlayerSubsystem);

	/**
	 * Calls BindToSettingsLocalPlayerSubsystem for each dynamic binding object on the given Class if it supports input delegates.
	 * 
	 * @param InClass				The class that will should be used to determine if Input Delegates are supported
	 * @param ObjectToBindTo		The UObject that the binding should use. Should not be null
	 */
	UFUNCTION(BlueprintCallable)
	static void BindEnhancedSettingDelegates(const UClass* InClass, USubsystem* Subsystem, UObject* ObjectToBindTo = nullptr);
		
	/**
	 * Will bind input delegates for the given Actor and traverse it's subobjects attempting to bind
	 * each of them
	 */
	UFUNCTION(BlueprintCallable)
	static void BindEnhancedSettingWithSubobjects(AActor* InActor, USubsystem* Subsystem);
protected:
	static TSet<UClass*> SettingBindingClasses;
};

UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingDelegateBinding : public USettingDelegateBinding
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	TArray<FBlueprintEnhancedSettingBinding> SettingDelegateBindings;

	//~ Begin USettingDelegateBinding Interface
	virtual void BindToSubsystem(USubsystem* Subsystem, UObject* ObjectToBindTo) const override;
	virtual void BindToLocalPlayerSubsystem(UEnhancedSettingsLocalPlayerSubsystem* SettingsLocalPlayerSubsystem, UObject* ObjectToBindTo) const override;
	static void TryToBindToSubsystem(USubsystem* Subsystem, const FBlueprintEnhancedSettingBinding& Binding, UObject* ObjectToBindTo);
	//~ End USettingDelegateBinding Interface
};

#if UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#endif
