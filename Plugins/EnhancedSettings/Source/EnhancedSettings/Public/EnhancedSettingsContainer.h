// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EnhancedSetting.h"
#include "EnhancedSettingsTypes.h"
#include "EnhancedSettingsContainer.generated.h"

/** Delegate signature for action events. */
DECLARE_DELEGATE(FEnhancedSettingHandlerSignature);
//DECLARE_DYNAMIC_DELEGATE_ThreeParams(FEnhancedSettingHandlerDynamicSignature, FEnhancedSettingValue, SettingValue, FGameplayTag, GameplayTagID, const UEnhancedSetting*, SourceSetting);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FEnhancedSettingHandlerBoolSignature, bool, SettingValue, FGameplayTag, GameplayTagID, const UEnhancedSetting*, EnhancedSetting);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FEnhancedSettingHandlerIntSignature, int, SettingValue, FGameplayTag, GameplayTagID, const UEnhancedSetting*, EnhancedSetting);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FEnhancedSettingHandlerFloatSignature, float, SettingValue, FGameplayTag, GameplayTagID, const UEnhancedSetting*, EnhancedSetting);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FEnhancedSettingHandlerColorSignature, FColor, SettingValue, FGameplayTag, GameplayTagID, const UEnhancedSetting*, EnhancedSetting);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FEnhancedSettingHandlerTagSignature, FGameplayTag, SettingValue, FGameplayTag, GameplayTagID, const UEnhancedSetting*, EnhancedSetting);


// Used to force clone constructor calls only
enum class EEnhancedSettingBindingClone : uint8 { ForceClone };

/** Unified storage for both native and dynamic delegates with any signature  */
template<typename TSignature>
struct TEnhancedSettingUnifiedDelegate
{
protected:
	/** Holds the delegate to call. */
	TSharedPtr<TSignature> Delegate;

	/** Should this delegate fire with an Editor Script function guard? */
	bool bShouldFireWithEditorScriptGuard = false;

public:
	
	bool IsBound() const
	{
		return Delegate.IsValid() && Delegate->IsBound();
	}

	bool IsBoundToObject(void const* Object) const
	{
		return IsBound() && Delegate->IsBoundToObject(Object);
	}

	void Unbind()
	{
		if (Delegate)
		{
			Delegate->Unbind();
		}
	}

	void SetShouldFireWithEditorScriptGuard(const bool bNewValue) { bShouldFireWithEditorScriptGuard = bNewValue; }
	
	bool ShouldFireWithEditorScriptGuard() const { return bShouldFireWithEditorScriptGuard; }

	/** Binds a native delegate, hidden for script delegates */
	template<	typename UserClass,
				typename TSig = TSignature,
				typename... TVars>
	void BindDelegate(UserClass* Object, typename TSig::template TMethodPtr<UserClass, TVars...> Func, TVars... Vars)
	{
		Unbind();
		Delegate = MakeShared<TSig>(TSig::CreateUObject(Object, Func, Vars...));
	}

	/** Binds a script delegate on an arbitrary UObject, hidden for native delegates */
	template<	typename TSig = TSignature,
				typename = typename TEnableIf<TIsDerivedFrom<TSig, FScriptDelegate>::IsDerived || TIsDerivedFrom<TSig, FMulticastScriptDelegate>::IsDerived>::Type>
	void BindDelegate(UObject* Object, const FName FuncName)
	{
		Unbind();
		Delegate = MakeShared<TSig>();
		Delegate->BindUFunction(Object, FuncName);
	}

	/** Binds a lambda expression */
	template<typename TSig = TSignature, typename FunctorType, typename... VarTypes>
	void BindLambda(FunctorType&& InFunctor, VarTypes&&... Vars)
	{
		Unbind();
		Delegate = MakeShared<TSig>();
		Delegate->BindLambda(Forward<FunctorType>(InFunctor), Forward<VarTypes>(Vars)...);
	}

	template<typename TSig = TSignature>
	TSig& MakeDelegate()
	{
		Unbind();
		Delegate = MakeShared<TSig>();
		return *Delegate;
	}

	template<typename... TArgs>
	void Execute(TArgs... Args) const
	{
		if (IsBound())
		{
			if (bShouldFireWithEditorScriptGuard)
			{
				FEditorScriptExecutionGuard ScriptGuard;
				Delegate->Execute(Args...);
			}
			else
			{
				Delegate->Execute(Args...);	
			}
		}
	}
};

/** A basic binding unique identifier */
struct FSettingBindingHandle
{
private:
    uint32 Handle = 0;

protected:
    ENHANCEDSETTINGS_API FSettingBindingHandle() {}

    // Generates a handle
    FSettingBindingHandle(const FSettingBindingHandle& CloneFrom, EEnhancedSettingBindingClone) : Handle(CloneFrom.Handle) {}	// Clones a handle

public:
    virtual ~FSettingBindingHandle() = default;

    bool operator==(const FSettingBindingHandle& Rhs) const
    {
        return (GetHandle() == Rhs.GetHandle());
    }

    uint32 GetHandle() const { return Handle; }
};

/** A binding to an input action trigger event. */
struct FEnhancedSettingEventBinding : public FSettingBindingHandle
{
private:

    /** Action against which we are bound */
    TWeakObjectPtr<UEnhancedSetting> Setting;

    /** Trigger event that raises the delegate */
    EEnhancedSettingChangeEvent TriggerEvent = EEnhancedSettingChangeEvent::None;

protected:
    // Clone constructor
    FEnhancedSettingEventBinding(const FEnhancedSettingEventBinding& CloneFrom, EEnhancedSettingBindingClone Clone) : FSettingBindingHandle(CloneFrom, Clone), Setting(CloneFrom.Setting), TriggerEvent(CloneFrom.TriggerEvent) {}

public:
    FEnhancedSettingEventBinding() = default;
    FEnhancedSettingEventBinding(UEnhancedSetting* InSetting, const EEnhancedSettingChangeEvent InTriggerEvent) : Setting(InSetting), TriggerEvent(InTriggerEvent) {}

    UEnhancedSetting* GetSetting() const { return Setting.Get(); }
    EEnhancedSettingChangeEvent GetChangeEvent() const { return TriggerEvent; }

    virtual void Execute(const FEnhancedSettingInstance& SettingData) const = 0;
    virtual TUniquePtr<FEnhancedSettingEventBinding> Clone() const = 0;
    virtual void SetShouldFireWithEditorScriptGuard(const bool bNewValue) = 0;
    virtual bool IsBoundToObject(void const* Object) const = 0;
};

template<typename TSignature>
struct FEnhancedSettingEventDelegateBinding : FEnhancedSettingEventBinding
{
private:
    FEnhancedSettingEventDelegateBinding(const FEnhancedSettingEventDelegateBinding<TSignature>& CloneFrom, EEnhancedSettingBindingClone Clone)
        : FEnhancedSettingEventBinding(CloneFrom, Clone), Delegate(CloneFrom.Delegate)
    {
        Delegate.SetShouldFireWithEditorScriptGuard(CloneFrom.Delegate.ShouldFireWithEditorScriptGuard());
    }
public:
    FEnhancedSettingEventDelegateBinding(UEnhancedSetting* Action, EEnhancedSettingChangeEvent InTriggerEvent)
        : FEnhancedSettingEventBinding(Action, InTriggerEvent)
    {}

    // Implemented below.
    virtual void Execute(const FEnhancedSettingInstance& SettingData) const override;
    virtual TUniquePtr<FEnhancedSettingEventBinding> Clone() const override
    {
        return TUniquePtr<FEnhancedSettingEventBinding>(new FEnhancedSettingEventDelegateBinding<TSignature>(*this, EEnhancedSettingBindingClone::ForceClone));
    }

    virtual void SetShouldFireWithEditorScriptGuard(const bool bNewValue) override
    {
        Delegate.SetShouldFireWithEditorScriptGuard(bNewValue);
    }

    virtual bool IsBoundToObject(void const* Object) const override
    {
        return Delegate.IsBoundToObject(Object);
    }

    TEnhancedSettingUnifiedDelegate<TSignature> Delegate;
};

/** Binds an action value for later reference. CurrentValue will be kept up to date with the value of the bound action */
struct FEnhancedSettingValueBinding : public FSettingBindingHandle
{
private:
    friend class UEnhancedPlayerInput;

    /** Action against which we are bound */
    TWeakObjectPtr<UEnhancedSetting> Setting;

    /** Copy of the current value of the action */
    mutable FEnhancedSettingValue CurrentValue;

public:
    FEnhancedSettingValueBinding() = default;
    FEnhancedSettingValueBinding(UEnhancedSetting* InSetting) : Setting(InSetting) {}

    UEnhancedSetting* GetAction() const { return Setting.Get(); }
    FEnhancedSettingValue GetValue() const { return CurrentValue; }
};

// Specific delegates
template<>
inline void FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerBoolSignature>::Execute(const FEnhancedSettingInstance& SettingData) const
{
	Delegate.Execute(SettingData.Value.BooleanValue, SettingData.Setting->GetUniqueSettingID(), SettingData.Setting.Get());
}

template<>
inline void FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerIntSignature>::Execute(const FEnhancedSettingInstance& SettingData) const
{
	Delegate.Execute(SettingData.Value.IntegerValue, SettingData.Setting->GetUniqueSettingID(), SettingData.Setting.Get());
}

template<>
inline void FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerFloatSignature>::Execute(const FEnhancedSettingInstance& SettingData) const
{
	Delegate.Execute(SettingData.Value.FloatValue, SettingData.Setting->GetUniqueSettingID(), SettingData.Setting.Get());
}

template<>
inline void FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerColorSignature>::Execute(const FEnhancedSettingInstance& SettingData) const
{
	Delegate.Execute(SettingData.Value.ColorValue, SettingData.Setting->GetUniqueSettingID(), SettingData.Setting.Get());
}

template<>
inline void FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerTagSignature>::Execute(const FEnhancedSettingInstance& SettingData) const
{
	Delegate.Execute(SettingData.Value.TagValue, SettingData.Setting->GetUniqueSettingID(), SettingData.Setting.Get());
}

/**
 * Storage and functionality for Enhanced Settings
 */
UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingsContainer : public UObject
{
	GENERATED_BODY()
	
public:
	FEnhancedSettingEventBinding& BindEnhancedSetting(UEnhancedSetting* Setting, EEnhancedSettingChangeEvent Event, UObject* Object, FName FunctionName);
	
	UFUNCTION(BlueprintCallable)
	void CallOnLoadedEvents();
private:
	/** The collection of Enhanced Setting bindings. */
	TArray<TUniquePtr<FEnhancedSettingEventBinding>> EnhancedActionEventBindings;
	
	int MaxProfiles = 5;
	int ProfileIndex = 1;

public:
	void SetProfile(const int Index);

	void Tick(float DeltaTime);
#pragma region Initialize
private:
	FEnhancedSettingValueContainer CurrentValueContainer;
	
	UPROPERTY(Transient)
	TMap<TObjectPtr<UEnhancedSetting>, FEnhancedSettingValue> PendingChanges;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UEnhancedSetting>> SettingsAssets;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEnhancedSetting>> ChangedSettingAssets;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEnhancedSettingWrapper>> ChangedSettingWrapperAssets;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UEnhancedSettingWrapper>> PendingSettingWrapperAssets;
	
	UFUNCTION(BlueprintCallable)
	void ApplyStack(const FGameplayTag& StackTag);

	UFUNCTION(BlueprintCallable)
	void RemoveStack(const FGameplayTag& StackTag);
	
	bool bUseAsyncLoading = false;
	void OnAssetRegistryLoaded();
	void LoadSettingsAssets();
	void OnSettingsAssetsLoadedAsync();
	bool TryToAddSettingToContainer(UEnhancedSetting* Setting);
	void AddSettingToContainer(UEnhancedSetting* Setting);
	void PostGatherSettings();
	
public:
	void AddEnhancedSettingWrapperChangedSetting(UEnhancedSettingWrapper* Setting);
	void RemoveEnhancedSettingWrapperChangedSetting(UEnhancedSettingWrapper* Setting);
	void AddPendingEnhancedSettingWrapper(UEnhancedSettingWrapper* Setting);
	void RemovePendingEnhancedSettingWrapper(UEnhancedSettingWrapper* Setting);

	void Initialize();
	
	FEnhancedSettingValueContainer GetSettingProfile()
	{
		return CurrentValueContainer;
	}
#pragma endregion
	
#pragma region Utility
private:
	template <typename T>
	T GetBoundValue(const TEnhancedSettingValues<T>& ValueContainer, FGameplayTag Key, const T& DefaultValue = T()) const;

public:
	// Getters
	bool GetBoundBoolValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting) const;
	int GetBoundIntValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting) const;
	float GetBoundFloatValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting) const;
	FColor GetBoundColorValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting) const;
	FGameplayTag GetBoundTagValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting) const;
	FEnhancedSettingValue GetBoundValue(const APlayerController* PlayerController, const UEnhancedSetting* Setting) const;

	// Setters
	void ChangeBoundBoolValue(UEnhancedSetting* Setting, const bool NewValue);
	void ChangeBoundIntValue(UEnhancedSetting* Setting, const int NewValue);
	void ChangeBoundFloatValue(UEnhancedSetting* Setting, const float NewValue);
	void ChangeBoundColorValue(UEnhancedSetting* Setting, const FColor NewValue);
	void ChangeBoundTagValue(UEnhancedSetting* Setting, const FGameplayTag NewValue);
	
	// Utility
	bool HasUnappliedChanges() const;
	void ClearUnappliedChanges();
	void ApplyChanges();
	void ResetSettings();
	void ResetSettingsAndApply();
	bool CanResetSettings() const;
private:
#pragma endregion 

	
#pragma region Save/Load
private:
	// Helper methods for serialization
	bool SerializeToJSON(FString& OutJson, const FEnhancedSettingValueContainer& ValueContainer) const;
	bool DeserializeFromJSON(const FString& InJson, FEnhancedSettingValueContainer& ValueContainer, const bool bIsBaseProfile);
	
	UEnhancedSetting* GetSettingByUniqueID(const FGameplayTag& UniqueID) const;

	FString GetSaveLoadFilePath(const bool bIsStack = false, FGameplayTag StackTag = FGameplayTag()) const;
	
public:
	void SaveStack(const FEnhancedSettingValueContainer& ValueContainer, const FGameplayTag& StackTag) const;
	bool LoadStack(FEnhancedSettingValueContainer& ValueContainer, const FGameplayTag& StackTag);
	
	void SaveSettings() const;
	bool LoadSettings();
#pragma endregion
};