// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnhancedSettingsContainer.h"
#include "EnhancedSetting.h"
#include "EnhancedSettingsContainerAccessor.h"
#include "Engine/AssetManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Kismet/KismetMathLibrary.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#pragma region Initialize
void UEnhancedSettingsContainer::Initialize()
{
	const FString DebugMessage = GetOuter()->GetClass()->GetFName().ToString() + "Settings - Initialized";
	UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMessage);

	// Start loading settings assets
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddUObject(this, &UEnhancedSettingsContainer::OnAssetRegistryLoaded);
		const FString IsLoading = "Asset registry is still loading assets, Queuing";
		UE_LOG(LogTemp, Log, TEXT("%s"), *IsLoading);
	}
	else
	{
		LoadSettingsAssets();
	}
}

FEnhancedSettingEventBinding& UEnhancedSettingsContainer::BindEnhancedSetting(
	UEnhancedSetting* Setting, EEnhancedSettingChangeEvent Event, UObject* Object, FName FunctionName)
{
	switch (Setting->GetSettingValueType())
	{
	default:
		checkf(false, TEXT(""))

		break;
	case EEnhancedSettingValueType::Boolean:
		{
			TUniquePtr<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerBoolSignature>> AB =
				MakeUnique<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerBoolSignature>>(Setting, Event);
			AB->Delegate.BindDelegate(Object, FunctionName);
			AB->Delegate.SetShouldFireWithEditorScriptGuard(true);

			return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));
		}
	case EEnhancedSettingValueType::Integer:
		{
			TUniquePtr<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerIntSignature>> AB =
				MakeUnique<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerIntSignature>>(Setting, Event);
			AB->Delegate.BindDelegate(Object, FunctionName);
			AB->Delegate.SetShouldFireWithEditorScriptGuard(true);

			return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));
		}
	case EEnhancedSettingValueType::Float:
		{
			TUniquePtr<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerFloatSignature>> AB =
				MakeUnique<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerFloatSignature>>(Setting, Event);
			AB->Delegate.BindDelegate(Object, FunctionName);
			AB->Delegate.SetShouldFireWithEditorScriptGuard(true);

			return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));
		}
	case EEnhancedSettingValueType::Color:
		{
			TUniquePtr<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerColorSignature>> AB =
				MakeUnique<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerColorSignature>>(Setting, Event);
			AB->Delegate.BindDelegate(Object, FunctionName);
			AB->Delegate.SetShouldFireWithEditorScriptGuard(true);

			return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));
		}
	case EEnhancedSettingValueType::Tag:
		{
			TUniquePtr<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerTagSignature>> AB =
				MakeUnique<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerTagSignature>>(Setting, Event);
			AB->Delegate.BindDelegate(Object, FunctionName);
			AB->Delegate.SetShouldFireWithEditorScriptGuard(true);

			return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));
		}
	}

	checkf(false, TEXT("Unexpected"));

	TUniquePtr<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerTagSignature>> AB =
		MakeUnique<FEnhancedSettingEventDelegateBinding<FEnhancedSettingHandlerTagSignature>>(Setting, Event);
	AB->Delegate.BindDelegate(Object, FunctionName);
	AB->Delegate.SetShouldFireWithEditorScriptGuard(true);

	return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));
}

void UEnhancedSettingsContainer::CallOnLoadedEvents()
{
	// Go through each setting and call the loaded delegate (current profile)
	for (auto& SA : CurrentValueContainer.BoolValues.Values)
	{
		if (UEnhancedSetting* Setting = GetSettingByUniqueID(SA.Key))
		{
			for (const auto& Binding : EnhancedActionEventBindings)
			{
				if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
					EEnhancedSettingChangeEvent::Loaded) Binding->Execute(FEnhancedSettingInstance(Setting, SA.Value));
			}
		}
	}
	for (auto& SA : CurrentValueContainer.IntegerValues.Values)
	{
		if (UEnhancedSetting* Setting = GetSettingByUniqueID(SA.Key))
		{
			for (const auto& Binding : EnhancedActionEventBindings)
			{
				if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
					EEnhancedSettingChangeEvent::Loaded) Binding->Execute(FEnhancedSettingInstance(Setting, SA.Value));
			}
		}
	}
	for (auto& SA : CurrentValueContainer.FloatValues.Values)
	{
		if (UEnhancedSetting* Setting = GetSettingByUniqueID(SA.Key))
		{
			for (const auto& Binding : EnhancedActionEventBindings)
			{
				if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
					EEnhancedSettingChangeEvent::Loaded) Binding->Execute(FEnhancedSettingInstance(Setting, SA.Value));
			}
		}
	}
	for (auto& SA : CurrentValueContainer.ColorValues.Values)
	{
		if (UEnhancedSetting* Setting = GetSettingByUniqueID(SA.Key))
		{
			for (const auto& Binding : EnhancedActionEventBindings)
			{
				if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
					EEnhancedSettingChangeEvent::Loaded) Binding->Execute(FEnhancedSettingInstance(Setting, SA.Value));
			}
		}
	}
	for (auto& SA : CurrentValueContainer.TagValues.Values)
	{
		if (UEnhancedSetting* Setting = GetSettingByUniqueID(SA.Key))
		{
			for (const auto& Binding : EnhancedActionEventBindings)
			{
				if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
					EEnhancedSettingChangeEvent::Loaded) Binding->Execute(FEnhancedSettingInstance(Setting, SA.Value));
			}
		}
	}
}

void UEnhancedSettingsContainer::AddPendingEnhancedSettingWrapper(UEnhancedSettingWrapper* Setting)
{
    PendingSettingWrapperAssets.AddUnique(Setting);
}

void UEnhancedSettingsContainer::RemovePendingEnhancedSettingWrapper(UEnhancedSettingWrapper* Setting)
{
    PendingSettingWrapperAssets.Remove(Setting);
}

void UEnhancedSettingsContainer::AddEnhancedSettingWrapperChangedSetting(UEnhancedSettingWrapper* Setting)
{
	ChangedSettingWrapperAssets.AddUnique(Setting);
}

void UEnhancedSettingsContainer::RemoveEnhancedSettingWrapperChangedSetting(UEnhancedSettingWrapper* Setting)
{
	ChangedSettingWrapperAssets.Remove(Setting);
}

void UEnhancedSettingsContainer::Tick(float DeltaTime)
{
	// Print all the pending wrapper changes

	for (auto& Setting : PendingSettingWrapperAssets)
	{
		FString Str = "Pending Wrapper: " + Setting->GetFName().ToString();
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, Str);
	}
	if (true) return;

	// Print all the pending changes
	for (auto& Change : PendingChanges) {
		FString Str = "Pending: " + Change.Key->GetUniqueSettingID().ToString();
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, Str);
	}
	// Print all the changed settings
	for (auto& Setting : ChangedSettingAssets){
		FString Str = "Changed: " + Setting->GetUniqueSettingID().ToString();
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, Str);
	}
}

void UEnhancedSettingsContainer::ApplyStack(const FGameplayTag& StackTag)
{
	// Load the stack profile
	FEnhancedSettingValueContainer StackProfile;
	if (!LoadStack(StackProfile,StackTag))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to load stack profile: %s"), *StackTag.ToString());
		return;
	}
}

void UEnhancedSettingsContainer::RemoveStack(const FGameplayTag& StackTag)
{
	
}

void UEnhancedSettingsContainer::OnAssetRegistryLoaded()
{
	LoadSettingsAssets();
}

void UEnhancedSettingsContainer::LoadSettingsAssets()
{
	UE_LOG(LogTemp, Log, TEXT("LoadSettingsAssets called."));
	if (!UAssetManager::IsInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("AssetManager is not initialized."));
		return;
	}
	const UAssetManager& AssetManager = UAssetManager::Get();
	const FPrimaryAssetType SettingsAssetType = FPrimaryAssetType("EnhancedSetting");

	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(SettingsAssetType, AssetIds);

	// Convert PrimaryAssetId to SoftObjectPath
	TArray<FSoftObjectPath> AssetPaths;
	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		AssetPaths.Add(AssetPath);
	}

	// Log the number of assets found
	UE_LOG(LogTemp, Log, TEXT("Found %d Setting assets in %s"), AssetPaths.Num(), *GetOuter()->GetClass()->GetName());

	// Log each AssetPath for verification
	for (const FSoftObjectPath& Path : AssetPaths)
	{
		UE_LOG(LogTemp, Log, TEXT("Loading Setting Asset: %s"), *Path.ToString());
	}

	if (bUseAsyncLoading)
	{
		// Request asynchronous load
		FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
		StreamableManager.RequestAsyncLoad(
			AssetPaths, FStreamableDelegate::CreateUObject(
				this, &UEnhancedSettingsContainer::OnSettingsAssetsLoadedAsync));
	}
	else
	{
		// Load the assets synchronously
		for (const FSoftObjectPath& Path : AssetPaths)
		{
			TryToAddSettingToContainer(Cast<UEnhancedSetting>(Path.TryLoad()));
		}

		PostGatherSettings();
	}
}

void UEnhancedSettingsContainer::OnSettingsAssetsLoadedAsync()
{
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, "On Settings Assets Loaded");

	UE_LOG(LogTemp, Log, TEXT("OnSettingsAssetsLoaded called."));

	// Populate SettingProfile.Values after assets are loaded
	const UAssetManager& AssetManager = UAssetManager::Get();
	const FPrimaryAssetType SettingsAssetType = TEXT("EnhancedSetting");

	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(SettingsAssetType, AssetIds);

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		TryToAddSettingToContainer(Cast<UEnhancedSetting>(AssetManager.GetPrimaryAssetPath(AssetId).TryLoad()));
	}

	PostGatherSettings();
}

void UEnhancedSettingsContainer::PostGatherSettings()
{
	LoadSettings();
}

bool UEnhancedSettingsContainer::TryToAddSettingToContainer(UEnhancedSetting* Setting)
{
	const EEnhancedSettingScope SettingScope = Cast<IEnhancedSettingsContainerAccessor>(GetOuter())->GetEnhancedSettingScope();
	if (IsValid(Setting) && Setting->GetEnhancedSettingScope() != SettingScope) return false;
	AddSettingToContainer(Setting);
	return true;
}

void UEnhancedSettingsContainer::AddSettingToContainer(UEnhancedSetting* Setting)
{
	SettingsAssets.Add(Setting);

	switch (Setting->GetSettingValueType())
	{
	case EEnhancedSettingValueType::Boolean:
		CurrentValueContainer.BoolValues.Values.Add(Setting->GetUniqueSettingID(), Cast<UEnhancedSettingBool>(Setting)->GetDefaultValue());
		break;
	case EEnhancedSettingValueType::Integer:
		CurrentValueContainer.IntegerValues.Values.Add(Setting->GetUniqueSettingID(), Cast<UEnhancedSettingInt>(Setting)->GetDefaultValue());
		break;
	case EEnhancedSettingValueType::Float:
		CurrentValueContainer.FloatValues.Values.Add(Setting->GetUniqueSettingID(), Cast<UEnhancedSettingFloat>(Setting)->GetDefaultValue());
		break;
	case EEnhancedSettingValueType::Color:
		CurrentValueContainer.ColorValues.Values.Add(Setting->GetUniqueSettingID(), Cast<UEnhancedSettingColor>(Setting)->GetDefaultValue());
		break;
	case EEnhancedSettingValueType::Tag:
		CurrentValueContainer.TagValues.Values.Add(Setting->GetUniqueSettingID(), Cast<UEnhancedSettingTag>(Setting)->GetDefaultValue());
		break;

	default:
		// Crash if the setting type is not handled
		checkf(false, TEXT("Setting type not handled"));
	}

	for (auto& Sett : CurrentValueContainer.FloatValues.Values)
	{
		FString Str = "Found: " + Sett.Key.ToString() + " " + FString::SanitizeFloat(Sett.Value);
		UE_LOG(LogTemp, Log, TEXT("Text %s"), *Str);
	}
}
#pragma endregion

#pragma region Getters
template <typename T>
T UEnhancedSettingsContainer::GetBoundValue(const TEnhancedSettingValues<T>& ValueContainer, FGameplayTag Key,
                                            const T& DefaultValue) const
{
	return CurrentValueContainer.GetBoundValue<ValueContainer>(Key, DefaultValue);;
}

bool UEnhancedSettingsContainer::GetBoundBoolValue(const APlayerController* PlayerController,
                                                   const UEnhancedSetting* Setting) const
{
	return CurrentValueContainer.GetBoundValue(CurrentValueContainer.BoolValues, Setting->GetUniqueSettingID(), false);
}

int UEnhancedSettingsContainer::GetBoundIntValue(const APlayerController* PlayerController,
                                                 const UEnhancedSetting* Setting) const
{
	return CurrentValueContainer.GetBoundValue(CurrentValueContainer.IntegerValues, Setting->GetUniqueSettingID(), -1);
}

float UEnhancedSettingsContainer::GetBoundFloatValue(const APlayerController* PlayerController,
                                                     const UEnhancedSetting* Setting) const
{
	return CurrentValueContainer.GetBoundValue(CurrentValueContainer.FloatValues, Setting->GetUniqueSettingID(), -1.f);
}

FColor UEnhancedSettingsContainer::GetBoundColorValue(const APlayerController* PlayerController,
                                                      const UEnhancedSetting* Setting) const
{
	return CurrentValueContainer.GetBoundValue(CurrentValueContainer.ColorValues, Setting->GetUniqueSettingID(), FColor(255, 0, 0));
}

FGameplayTag UEnhancedSettingsContainer::GetBoundTagValue(const APlayerController* PlayerController,
                                                          const UEnhancedSetting* Setting) const
{
	return CurrentValueContainer.GetBoundValue(CurrentValueContainer.TagValues, Setting->GetUniqueSettingID(), FGameplayTag());
}

FEnhancedSettingValue UEnhancedSettingsContainer::GetBoundValue(const APlayerController* PlayerController,
                                                                const UEnhancedSetting* Setting) const
{
	switch (Setting->GetSettingValueType())
	{
	case EEnhancedSettingValueType::Boolean:
		return FEnhancedSettingValue(GetBoundBoolValue(PlayerController, Setting));

	case EEnhancedSettingValueType::Integer:
		return FEnhancedSettingValue(GetBoundIntValue(PlayerController, Setting));

	case EEnhancedSettingValueType::Float:
		return FEnhancedSettingValue(GetBoundFloatValue(PlayerController, Setting));

	case EEnhancedSettingValueType::Color:
		return FEnhancedSettingValue(GetBoundColorValue(PlayerController, Setting));

	case EEnhancedSettingValueType::Tag:
		return FEnhancedSettingValue(GetBoundTagValue(PlayerController, Setting));

	default:
		checkf(false, TEXT("SETTING VALUE TYPE IS UNHANDLED"))
		break;
	}
	return FEnhancedSettingValue();
}

void UEnhancedSettingsContainer::ChangeBoundBoolValue(UEnhancedSetting* Setting, const bool NewValue)
{
	for (const auto& Binding : EnhancedActionEventBindings)
	{
		if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
			EEnhancedSettingChangeEvent::Changed) Binding->Execute(FEnhancedSettingInstance(Setting, NewValue));
	}
	
	// Ensure the value is not the same as the current value in the map
	if (CurrentValueContainer.BoolValues.Values.Contains(Setting->GetUniqueSettingID()) &&
		CurrentValueContainer.BoolValues.Values[Setting->GetUniqueSettingID()] == NewValue)
	{
		PendingChanges.Remove(Setting);
		return;
	}
	PendingChanges.Add(Setting, FEnhancedSettingValue(NewValue));
}

void UEnhancedSettingsContainer::ChangeBoundIntValue(UEnhancedSetting* Setting, const int NewValue)
{
	for (const auto& Binding : EnhancedActionEventBindings)
	{
		if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
			EEnhancedSettingChangeEvent::Changed) Binding->Execute(FEnhancedSettingInstance(Setting, NewValue));
	}
	
	// Ensure the value is not the same as the current value in the map
	if (CurrentValueContainer.IntegerValues.Values.Contains(Setting->GetUniqueSettingID()) &&
		CurrentValueContainer.IntegerValues.Values[Setting->GetUniqueSettingID()] == NewValue)
	{
		PendingChanges.Remove(Setting);
		return;
	}
	PendingChanges.Add(Setting, FEnhancedSettingValue(NewValue));
}

void UEnhancedSettingsContainer::ChangeBoundFloatValue(UEnhancedSetting* Setting, const float NewValue)
{	
	for (const auto& Binding : EnhancedActionEventBindings)
	{
		if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
			EEnhancedSettingChangeEvent::Changed) Binding->Execute(FEnhancedSettingInstance(Setting, NewValue));
	}
	
	// Ensure the value is not the same as the current value in the map
	if (CurrentValueContainer.FloatValues.Values.Contains(Setting->GetUniqueSettingID()) &&
		CurrentValueContainer.FloatValues.Values[Setting->GetUniqueSettingID()] == NewValue)
	{
		PendingChanges.Remove(Setting);
		return;
	}
	PendingChanges.Add(Setting, FEnhancedSettingValue(NewValue));
}

void UEnhancedSettingsContainer::ChangeBoundColorValue(UEnhancedSetting* Setting, const FColor NewValue)
{
	for (const auto& Binding : EnhancedActionEventBindings)
	{
		if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
			EEnhancedSettingChangeEvent::Changed) Binding->Execute(FEnhancedSettingInstance(Setting, NewValue));
	}
	
	// Ensure the value is not the same as the current value in the map
	if (CurrentValueContainer.ColorValues.Values.Contains(Setting->GetUniqueSettingID()) &&
		CurrentValueContainer.ColorValues.Values[Setting->GetUniqueSettingID()] == NewValue)
	{
		PendingChanges.Remove(Setting);
		return;
	}
	PendingChanges.Add(Setting, FEnhancedSettingValue(NewValue));
}

void UEnhancedSettingsContainer::ChangeBoundTagValue(UEnhancedSetting* Setting, const FGameplayTag NewValue)
{
	for (const auto& Binding : EnhancedActionEventBindings)
	{
		if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
			EEnhancedSettingChangeEvent::Changed) Binding->Execute(FEnhancedSettingInstance(Setting, NewValue));
	}
	
	// Ensure the value is not the same as the current value in the map
	if (CurrentValueContainer.TagValues.Values.Contains(Setting->GetUniqueSettingID()) &&
		CurrentValueContainer.TagValues.Values[Setting->GetUniqueSettingID()] == NewValue)
	{
		PendingChanges.Remove(Setting);
		return;
	}
	
	PendingChanges.Add(Setting, FEnhancedSettingValue(NewValue));
}

bool UEnhancedSettingsContainer::HasUnappliedChanges() const
{
	return PendingChanges.Num() > 0 || PendingSettingWrapperAssets.Num() > 0;
}

void UEnhancedSettingsContainer::ClearUnappliedChanges()
{
	for (auto& Change : PendingChanges)
	{
		for (const auto& Binding : EnhancedActionEventBindings)
		{
			if (Binding.IsValid() && Binding->GetSetting() == Change.Key && Binding->GetChangeEvent() ==
				EEnhancedSettingChangeEvent::Changed) Binding->Execute(
				FEnhancedSettingInstance(Change.Key.Get(), Change.Value));
		}
	}
	PendingChanges.Empty();

	for (auto& Setting : PendingSettingWrapperAssets)
    {
        Setting->GetAccessor()->ClearValue(GEngine->GetFirstLocalPlayerController(GEngine->GetWorld()));
    }
	PendingSettingWrapperAssets.Empty();	
}

void UEnhancedSettingsContainer::ApplyChanges()
{
	for (auto& Change : PendingChanges)
	{
		UEnhancedSetting* Setting = Change.Key.Get();
		FGameplayTag Key = Change.Key->GetUniqueSettingID();
		switch (Change.Key->GetSettingValueType())
		{
		case EEnhancedSettingValueType::Boolean:
			CurrentValueContainer.BoolValues.Values.Add(Key, Change.Value.BooleanValue);
			Setting = Change.Key.Get();
			if (Cast<UEnhancedSettingBool>(Setting)->GetDefaultValue() == Change.Value.BooleanValue)
			{
				ChangedSettingAssets.Remove(Setting);
			}
			else
			{
				ChangedSettingAssets.AddUnique(Setting);
			}
			break;
		case EEnhancedSettingValueType::Integer:
			CurrentValueContainer.IntegerValues.Values.Add(Key, Change.Value.IntegerValue);

			if (Cast<UEnhancedSettingInt>(Setting)->GetDefaultValue() == Change.Value.IntegerValue)
			{
				ChangedSettingAssets.Remove(Setting);
			}
			else
			{
				ChangedSettingAssets.AddUnique(Setting);
			}
			break;
		case EEnhancedSettingValueType::Float:
			CurrentValueContainer.FloatValues.Values.Add(Key, Change.Value.FloatValue);

			if (Cast<UEnhancedSettingFloat>(Setting)->GetDefaultValue() == Change.Value.FloatValue)
			{
				ChangedSettingAssets.Remove(Setting);
			}
			else
			{
				ChangedSettingAssets.AddUnique(Setting);
			}
			break;
		case EEnhancedSettingValueType::Color:
			CurrentValueContainer.ColorValues.Values.Add(Key, Change.Value.ColorValue);

			if (Cast<UEnhancedSettingColor>(Setting)->GetDefaultValue() == Change.Value.ColorValue)
			{
				ChangedSettingAssets.Remove(Setting);
			}
			else
			{
				ChangedSettingAssets.AddUnique(Setting);
			}
			break;
		case EEnhancedSettingValueType::Tag:
			CurrentValueContainer.TagValues.Values.Add(Key, Change.Value.TagValue);

			if (Cast<UEnhancedSettingTag>(Setting)->GetDefaultValue() == Change.Value.TagValue)
			{
				ChangedSettingAssets.Remove(Setting);
			}
			else
			{
				ChangedSettingAssets.AddUnique(Setting);
			}
			break;

		default:
			checkf(false, TEXT("SETTING VALUE TYPE IS UNHANDLED"))
		}
		for (const auto& Binding : EnhancedActionEventBindings)
		{
			if (Binding.IsValid() && Binding->GetSetting() == Change.Key && Binding->GetChangeEvent() ==
				EEnhancedSettingChangeEvent::Commited) Binding->Execute(
				FEnhancedSettingInstance(Change.Key.Get(), Change.Value));
		}
	}
	
	PendingChanges.Empty();

	for (auto& Setting : PendingSettingWrapperAssets)
    {
        Setting->GetAccessor()->ApplyValue(nullptr);
    }

	PendingSettingWrapperAssets.Empty();
	
	SaveSettings();
}

void UEnhancedSettingsContainer::SetProfile(const int Index)
{
	ResetSettings();
	ProfileIndex = FMath::Clamp(Index, 1, MaxProfiles);
	LoadSettings();
	
	// Okay now we have applied the saved settings but the save file only stores changes, so there may be unchanged settings that are not in the save file
}

void UEnhancedSettingsContainer::ResetSettings()
{
	if (CanResetSettings())
	{
		for (auto& Setting : ChangedSettingAssets)
		{
			switch (Setting->GetSettingValueType())
			{
			case EEnhancedSettingValueType::Boolean:
				{
					UEnhancedSettingBool* BoolSetting = Cast<UEnhancedSettingBool>(Setting);
					CurrentValueContainer.BoolValues.Values.Add(Setting->GetUniqueSettingID(), BoolSetting->GetDefaultValue());
					for (const auto& Binding : EnhancedActionEventBindings)
					{
						if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
							EEnhancedSettingChangeEvent::Reset) Binding->Execute(
							FEnhancedSettingInstance(Setting.Get(), BoolSetting->GetDefaultValue()));
					}
				}
				break;
			case EEnhancedSettingValueType::Integer:
				{
					UEnhancedSettingInt* IntSetting = Cast<UEnhancedSettingInt>(Setting);
					CurrentValueContainer.IntegerValues.Values.Add(Setting->GetUniqueSettingID(), IntSetting->GetDefaultValue());
					for (const auto& Binding : EnhancedActionEventBindings)
					{
						if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
							EEnhancedSettingChangeEvent::Reset) Binding->Execute(
							FEnhancedSettingInstance(Setting.Get(), IntSetting->GetDefaultValue()));
					}
				}
				break;
			case EEnhancedSettingValueType::Float:
				{
					UEnhancedSettingFloat* FloatSetting = Cast<UEnhancedSettingFloat>(Setting);
					CurrentValueContainer.FloatValues.Values.Add(Setting->GetUniqueSettingID(), FloatSetting->GetDefaultValue());
					// Print the value of the setting
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Setting->GetUniqueSettingID().ToString());
					for (const auto& Binding : EnhancedActionEventBindings)
					{
						if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
							EEnhancedSettingChangeEvent::Reset) Binding->Execute(
							FEnhancedSettingInstance(Setting.Get(), FloatSetting->GetDefaultValue()));
					}
				}
				break;
			case EEnhancedSettingValueType::Color:
				{
					UEnhancedSettingColor* ColorSetting = Cast<UEnhancedSettingColor>(Setting);
					CurrentValueContainer.ColorValues.Values.Add(Setting->GetUniqueSettingID(), ColorSetting->GetDefaultValue());
					for (const auto& Binding : EnhancedActionEventBindings)
					{
						if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
							EEnhancedSettingChangeEvent::Reset) Binding->Execute(
							FEnhancedSettingInstance(Setting.Get(), ColorSetting->GetDefaultValue()));
					}
				}
				break;
			case EEnhancedSettingValueType::Tag:
				{
					UEnhancedSettingTag* TagSetting = Cast<UEnhancedSettingTag>(Setting);
					CurrentValueContainer.TagValues.Values.Add(Setting->GetUniqueSettingID(), TagSetting->GetDefaultValue());
					for (const auto& Binding : EnhancedActionEventBindings)
					{
						if (Binding.IsValid() && Binding->GetSetting() == Setting && Binding->GetChangeEvent() ==
							EEnhancedSettingChangeEvent::Reset) Binding->Execute(
							FEnhancedSettingInstance(Setting.Get(), TagSetting->GetDefaultValue()));
					}
				}
				break;
			default:
				checkf(false, TEXT("SETTING VALUE TYPE IS UNHANDLED"))
			}
		}
	}
	
	PendingChanges.Empty();
	ChangedSettingAssets.Empty();

	for (auto& Setting : ChangedSettingWrapperAssets)
    {
        Setting->GetAccessor()->ResetValue(GEngine->GetFirstLocalPlayerController(GEngine->GetWorld()));
    }

	PendingSettingWrapperAssets.Empty();
	ChangedSettingWrapperAssets.Empty();
}

void UEnhancedSettingsContainer::ResetSettingsAndApply()
{
	ResetSettings();
	SaveSettings();
}

bool UEnhancedSettingsContainer::CanResetSettings() const
{
	// Will need to evaluate what settings are on the current page, could pass in a list of settings to check
	// Could pass in the settings page data asset once you select it, then evaluate edit conditions, create a list of settings to check
	// Would need to re-evaluate if any edit conditions change
	return ChangedSettingAssets.Num() > 0 || ChangedSettingWrapperAssets.Num() > 0;
}
#pragma endregion

#pragma region SaveLoad
FString UEnhancedSettingsContainer::GetSaveLoadFilePath(const bool bIsStack, FGameplayTag StackTag) const
{
	// Get the name of the outer UObject
	FString OuterName = GetOuter() ? GetOuter()->GetClass()->GetName() : TEXT("Default");
	
	// Create the file path using the outer name
	FString FilePath;

	FString Profile = "Profile" + FString::FromInt(ProfileIndex);
#if WITH_EDITOR
	// For the Editor build
	FilePath = FPaths::ProjectDir() / TEXT("Saved/Settings/") / Profile / OuterName + TEXT(".json") + (bIsStack ? StackTag.ToString() : TEXT(""));
#else
    // For Development or Shipping builds
    FilePath = FPaths::ProjectUserDir() / TEXT("Settings/") / Profile / OuterName + TEXT(".json") + (bIsStack ? StackTag.ToString() : TEXT(""));
#endif

	// Ensure the directory exists
	if (const FString DirectoryPath = FPaths::GetPath(FilePath); !FPaths::DirectoryExists(DirectoryPath))
	{
		// Create the directory if it does not exist
		IFileManager::Get().MakeDirectory(*DirectoryPath, true);
	}

	return FilePath;
}

void UEnhancedSettingsContainer::SaveStack(const FEnhancedSettingValueContainer& ValueContainer, const FGameplayTag& StackTag) const
{
	FString JsonString;
	if (!SerializeToJSON(JsonString, ValueContainer))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to serialize settings to JSON."));
		return;
	}

	// Get the path for saving the settings
	const FString FilePath = GetSaveLoadFilePath(true, StackTag);
	if (!FFileHelper::SaveStringToFile(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to save settings to file: %s"), *FilePath);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Settings saved successfully to: %s"), *FilePath);
}

bool UEnhancedSettingsContainer::LoadStack(FEnhancedSettingValueContainer& ValueContainer, const FGameplayTag& StackTag)
{
	// Get the path for loading the settings
	const FString FilePath = GetSaveLoadFilePath(true, StackTag);;
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to load settings from file: %s"), *FilePath);
		return false;
	}

	if (!DeserializeFromJSON(JsonString, ValueContainer, false))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to deserialize settings from JSON."));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Settings loaded successfully from: %s"), *FilePath);
	return true;
}

void UEnhancedSettingsContainer::SaveSettings() const
{
	FString JsonString;
	if (!SerializeToJSON(JsonString, CurrentValueContainer))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to serialize settings to JSON."));
		return;
	}

	// Get the path for saving the settings
	const FString FilePath = GetSaveLoadFilePath();
	if (!FFileHelper::SaveStringToFile(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to save settings to file: %s"), *FilePath);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Settings saved successfully to: %s"), *FilePath);
}

bool UEnhancedSettingsContainer::LoadSettings()
{
	// Get the path for loading the settings
	const FString FilePath = GetSaveLoadFilePath();
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to load settings from file: %s"), *FilePath);
		return false;
	}

	if (!DeserializeFromJSON(JsonString, CurrentValueContainer, true))
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to deserialize settings from JSON."));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Settings loaded successfully from: %s"), *FilePath);
	return true;
}

bool UEnhancedSettingsContainer::SerializeToJSON(FString& OutJson, const FEnhancedSettingValueContainer& ValueContainer) const
{
	// Create a JSON writer to write to the output string
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);

	Writer->WriteObjectStart();

	Writer->WriteObjectStart("BoolValues");
	for (const auto& Elem : ValueContainer.BoolValues.Values)
	{
		// Ensure the value is not the default value
		if (Elem.Value == Cast<UEnhancedSettingBool>(GetSettingByUniqueID(Elem.Key))->GetDefaultValue())
		{
			continue;
		}
		Writer->WriteValue(Elem.Key.ToString(), Elem.Value);
	}
	Writer->WriteObjectEnd();

	Writer->WriteObjectStart("IntegerValues");
	for (const auto& Elem : ValueContainer.IntegerValues.Values)
	{
		// Ensure the value is not the default value
		if (Elem.Value ==  Cast<UEnhancedSettingInt>(GetSettingByUniqueID(Elem.Key))->GetDefaultValue())
		{
			continue;
		}
		Writer->WriteValue(Elem.Key.ToString(), Elem.Value);
	}
	Writer->WriteObjectEnd();

	Writer->WriteObjectStart("FloatValues");
	for (const auto& Elem : ValueContainer.FloatValues.Values)
	{
		// Ensure the value is not the default value
		if (Elem.Value == Cast<UEnhancedSettingFloat>(GetSettingByUniqueID(Elem.Key))->GetDefaultValue())
		{
			continue;
		}
		
		Writer->WriteValue(Elem.Key.ToString(), Elem.Value);
	}
	Writer->WriteObjectEnd();

	Writer->WriteObjectStart("ColorValues");
	for (const auto& Elem : ValueContainer.ColorValues.Values)
	{
		// Ensure the value is not the default value
		if (Elem.Value == Cast<UEnhancedSettingColor>(GetSettingByUniqueID(Elem.Key))->GetDefaultValue())
		{
			continue;
		}
		
		// Convert FColor to a string representation
		const FString ColorString = FString::Printf(
			TEXT("%hhu,%hhu,%hhu,%hhu"), Elem.Value.R, Elem.Value.G, Elem.Value.B, Elem.Value.A);
		Writer->WriteValue(Elem.Key.ToString(), ColorString);
	}
	Writer->WriteObjectEnd();

	Writer->WriteObjectStart("TagValues");
	for (const auto& Elem : ValueContainer.TagValues.Values)
	{
		// Ensure the value is not the default value
		if (Elem.Value == Cast<UEnhancedSettingTag>(GetSettingByUniqueID(Elem.Key))->GetDefaultValue())
		{
			continue;
		}
		
		Writer->WriteValue(Elem.Key.ToString(), Elem.Value.ToString());
	}
	Writer->WriteObjectEnd();

	// Close the main object
	Writer->WriteObjectEnd();

	// Close the writer and return the result
	return Writer->Close();
}

bool UEnhancedSettingsContainer::DeserializeFromJSON(const FString& InJson, FEnhancedSettingValueContainer& ValueContainer, const bool bIsBaseProfile)
{
	// Create a JSON reader to read from the input string
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InJson);
	TSharedPtr<FJsonObject> JsonObject;

	// Deserialize the JSON string into a JsonObject
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return false; // Failed to parse JSON
	}

	// Deserialize BoolValues
	if (JsonObject->HasField(TEXT("BoolValues")))
	{
		const TSharedPtr<FJsonObject> BoolValuesObject = JsonObject->GetObjectField(TEXT("BoolValues"));
		for (const auto& Elem : BoolValuesObject->Values)
		{
			FGameplayTag Key = FGameplayTag::RequestGameplayTag(FName(*Elem.Key));
			
			if (UEnhancedSetting* Setting = GetSettingByUniqueID(Key))
			{
				bool Value = Elem.Value->AsBool();
				ValueContainer.BoolValues.Values.Add(Key, Value);
				if (bIsBaseProfile) ChangedSettingAssets.Add(Setting);
			}
		}
	}

	// Deserialize IntegerValues
	if (JsonObject->HasField(TEXT("IntegerValues")))
	{
		const TSharedPtr<FJsonObject> IntegerValuesObject = JsonObject->GetObjectField(TEXT("IntegerValues"));
		for (const auto& Elem : IntegerValuesObject->Values)
		{
			FGameplayTag Key = FGameplayTag::RequestGameplayTag(FName(*Elem.Key));
			
			if (UEnhancedSetting* Setting = GetSettingByUniqueID(Key))
			{
				int32 Value = Elem.Value->AsNumber();
				ValueContainer.IntegerValues.Values.Add(Key, Value);
				if (bIsBaseProfile) ChangedSettingAssets.Add(Setting);
			}
		}
	}

	// Deserialize FloatValues
	if (JsonObject->HasField(TEXT("FloatValues")))
	{
		const TSharedPtr<FJsonObject> FloatValuesObject = JsonObject->GetObjectField(TEXT("FloatValues"));
		for (const auto& Elem : FloatValuesObject->Values)
		{
			FGameplayTag Key = FGameplayTag::RequestGameplayTag(FName(*Elem.Key));
			
			if (UEnhancedSetting* Setting = GetSettingByUniqueID(Key))
			{
				float Value = Elem.Value->AsNumber();
				ValueContainer.FloatValues.Values.Add(Key, Value);
				if (bIsBaseProfile) ChangedSettingAssets.Add(Setting);
			}
		}
	}

	// Deserialize ColorValues
	if (JsonObject->HasField(TEXT("ColorValues")))
	{
		const TSharedPtr<FJsonObject> ColorValuesObject = JsonObject->GetObjectField(TEXT("ColorValues"));
		for (const auto& Elem : ColorValuesObject->Values)
		{
			
			FGameplayTag Key = FGameplayTag::RequestGameplayTag(FName(*Elem.Key));
			
			if (UEnhancedSetting* Setting = GetSettingByUniqueID(Key))
			{
				FString ColorString = Elem.Value->AsString();

				// Split the color string into components (assuming format "R,G,B,A")
				TArray<FString> ColorComponents;
				ColorString.ParseIntoArray(ColorComponents, TEXT(","), true);

				if (ColorComponents.Num() == 4)
				{
					// Create FColor from the parsed components
					FColor ColorValue(
						FMath::Clamp(FCString::Atof(*ColorComponents[0]) * 255, 0.0f, 255.0f),
						FMath::Clamp(FCString::Atof(*ColorComponents[1]) * 255, 0.0f, 255.0f),
						FMath::Clamp(FCString::Atof(*ColorComponents[2]) * 255, 0.0f, 255.0f),
						FMath::Clamp(FCString::Atof(*ColorComponents[3]) * 255, 0.0f, 255.0f)
					);
					ValueContainer.ColorValues.Values.Add(Key, ColorValue);
				}
				if (bIsBaseProfile) ChangedSettingAssets.Add(Setting);
			}
		}
	}

	// Deserialize TagValues
	if (JsonObject->HasField(TEXT("TagValues")))
	{
		const TSharedPtr<FJsonObject> TagValuesObject = JsonObject->GetObjectField(TEXT("TagValues"));
		for (const auto& Elem : TagValuesObject->Values)
		{
			
			FGameplayTag Key = FGameplayTag::RequestGameplayTag(FName(*Elem.Key));
			
			if (UEnhancedSetting* Setting = GetSettingByUniqueID(Key))
			{
				FGameplayTag Value = FGameplayTag::RequestGameplayTag(FName(*Elem.Value->AsString()));
				ValueContainer.TagValues.Values.Add(Key, Value);
				if (bIsBaseProfile) ChangedSettingAssets.Add(Setting);
			}
		}
	}
	return true;
}

UEnhancedSetting* UEnhancedSettingsContainer::GetSettingByUniqueID(const FGameplayTag& UniqueID) const
{
	for (const auto& Setting : SettingsAssets)
	{
		if (Setting && Setting->GetUniqueSettingID() == UniqueID)
		{
			return Setting.Get();
		}
	}
	return nullptr;
}
#pragma endregion
