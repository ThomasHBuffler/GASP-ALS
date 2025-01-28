// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "EnhancedSettingsTypes.h"
#include "EnhancedSetting.generated.h"

/*
 * An instanced, edit-inline meta asset designed to store abstract data related to your settings. 
 * It can be used for user interface enhancements such as icons, sounds, or colors.
 *
 * Usage:
 * 1. **Create a Subclass:** 
 *    Start by subclassing this base class and adding the necessary properties (e.g., icons, sounds). 
 *    See the `UMDUserInterface` class for an example implementation.
 *
 * 2. **Configure in Two Ways:**
 *    a) **Inline Editing:**  
 *       Select the subclass directly within the USetting data asset definition and configure it inline.
 *       
 *    b) **Content Browser Instances:**  
 *       Create specific instances for individual settings (e.g., mouse sensitivity, invert Y-axis) in the content browser. 
 *       You can select these instances within the USetting data asset to reuse as prefabs for certain setting types.
 *
 * **Note:** 
 * You can also add new values in a Blueprint class, but this approach introduces complexity, 
 * such as needing to cast types and losing native C++ support for those new variables.
 * You could later migrate these values to the C++ class if needed, so decide based on your project's requirements.
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced, Abstract)
class UMetaDataDescriptorBase : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class UMDDUserInterface : public UMetaDataDescriptorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Descriptor")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Descriptor")
	TObjectPtr<USoundBase> MouseEnterSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Descriptor")
	TObjectPtr<USoundBase> MouseExitSound;	
};

// @TODO - Add edit conditions, Platform traits, Game modes, etc (Tag based)
// @TODO - Add subcategory support (maybe even a heirarchy of categories, using tags)

/*
 * A requirement that must be met for a setting to be available.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew, DefaultToInstanced)
class ENHANCEDSETTINGS_API UEnhancedSettingEditRequirement : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	bool IsRequirementMet(APlayerController* PlayerController) const;

	virtual bool IsRequirementMet_Implementation(APlayerController* PlayerController) const { return true; }
};

UCLASS(BlueprintType)
class ENHANCEDSETTINGS_API UESER_MainPlayer : public UEnhancedSettingEditRequirement
{
	GENERATED_BODY()

public:
	virtual bool IsRequirementMet_Implementation(APlayerController* PlayerController) const override;
};

/*
 * A filter that can be applied to a setting to restrict the available options.
 */
UCLASS(Blueprintable, Blueprintable, Abstract, EditInlineNew, DefaultToInstanced)
class ENHANCEDSETTINGS_API UEnhancedSettingOptionFilterBase : public UObject
{
	GENERATED_BODY()
};

/*
 * A filter that can be applied to a setting to restrict the available options. Integer.
 */
UCLASS(Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingOptionFilterInt : public UEnhancedSettingOptionFilterBase
{
	GENERATED_BODY()

public:
	// Limit the options to a range
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Filter")
	bool bLimitMinimum = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Filter", meta = (EditCondition = "bLimitMinimum"))
	int MinimumValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Filter")
	bool bLimitMaximum = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Filter", meta = (EditCondition = "bLimitMaximum"))
	int MaximumValue = 0;

	// Limit the options to a specific set of values
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Filter")
	bool bLimitOptionsExplicit = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Filter", meta = (EditCondition = "bLimitOptionsExplicit"))
	TArray<int> ExplicitOptions;
};

/*
 * Filter that can be applied to a setting to restrict the available options. Tag.
 */
UCLASS(Blueprintable)
class ENHANCEDSETTINGS_API UEnhancedSettingOptionFilterTag : public UEnhancedSettingOptionFilterBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Filter")
	FGameplayTagContainer SupportedTags;
};

/**
 * The base class for all settings.
 */
UCLASS(Abstract, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingBase : public UDataAsset
{
	GENERATED_BODY()

protected:
	// A localized name of this setting for user interface
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedSetting|Description")
	FText SettingName = FText::GetEmpty();
	
	// A localized descriptor of this setting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedSetting|Description", meta = (MultiLine))
	FText SettingDescription = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Description")
	TArray<TObjectPtr<UEnhancedSettingEditRequirement>> Requirements;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Description")
	TObjectPtr<UMetaDataDescriptorBase> MetaDescriptor;
	
public:
	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	virtual EEnhancedSettingScope GetEnhancedSettingScope() const { return LocalPlayer; }
		
	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	bool AreRequirementsMet(APlayerController* PlayerController) const;
};

/**
 * Base accessor for a UGameUserSettings other already defined settings.
*/
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew, DefaultToInstanced)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperAccessorBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	void ApplyValue(APlayerController* PlayerController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	void ClearValue(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	void ResetValue(APlayerController* PlayerController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	bool CanResetValue(APlayerController* PlayerController) const;
	
	virtual void ApplyValue_Implementation(APlayerController* PlayerController) {}

	virtual void ResetValue_Implementation(APlayerController* PlayerController) {}
	
	virtual void ClearValue_Implementation(APlayerController* PlayerController) {}

	virtual bool CanResetValue_Implementation(APlayerController* PlayerController) const { return false; }
};

/**
 * Bool accessor for a UGameUserSettings other already defined settings.
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperBoolAccessor : public UEnhancedSettingWrapperAccessorBase
{
	GENERATED_BODY()

protected:
	bool PendingValue = false;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	bool GetBoolValue() const;

	UFUNCTION(BlueprintCallable, Category = "EnhancedSetting|Accessor")
	bool GetPendingValue() const { return PendingValue; }
	
	virtual bool GetBoolValue_Implementation() const { return false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	void SetBoolValue(bool NewValue);

	virtual void SetBoolValue_Implementation(bool NewValue)
	{
		PendingValue = NewValue;
	}
};

/**
 * Int accessor for a UGameUserSettings other already defined settings.
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperIntAccessor : public UEnhancedSettingWrapperAccessorBase
{
	GENERATED_BODY()

protected:
	int PendingValue = 0;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	int GetIntValue() const;
	
	UFUNCTION(BlueprintCallable, Category = "EnhancedSetting|Accessor")
	int GetPendingValue() const { return PendingValue; }
	
	virtual int GetIntValue_Implementation() const { return 0; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	void SetIntValue(int NewValue);

	virtual void SetIntValue_Implementation(int NewValue)
	{
		PendingValue = NewValue;
	}
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	TArray<int> GetAvailableOptions() const;

	virtual TArray<int> GetAvailableOptions_Implementation() const { return TArray<int>(); }
};

/**
 * Float accessor for a UGameUserSettings other already defined settings.
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperFloatAccessor : public UEnhancedSettingWrapperAccessorBase
{
	GENERATED_BODY()

protected:
	float PendingValue = 0.0f;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	float GetFloatValue() const;
	
	UFUNCTION(BlueprintCallable, Category = "EnhancedSetting|Accessor")
	float GetPendingValue() const { return PendingValue; }
	
	virtual float GetFloatValue_Implementation() const { return 0.0f; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	void SetFloatValue(float NewValue);

	virtual void SetFloatValue_Implementation(float NewValue)
	{
		PendingValue = NewValue;
	}
};

/**
 * Int2D
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperInt2DAccessor : public UEnhancedSettingWrapperAccessorBase
{
	GENERATED_BODY()
	
protected:
	FIntPoint PendingValue = FIntPoint(0, 0);
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	FIntPoint GetInt2DValue() const;
	
	UFUNCTION(BlueprintCallable, Category = "EnhancedSetting|Accessor")
	FIntPoint GetPendingValue() const { return PendingValue; }
	
	virtual FIntPoint GetInt2DValue_Implementation() const { return FIntPoint(0, 0); }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	void SetInt2DValue(FIntPoint NewValue, APlayerController* PlayerController);

	virtual void SetInt2DValue_Implementation(FIntPoint NewValue, APlayerController* PlayerController)
	{
		PendingValue = NewValue;
	}
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EnhancedSetting|Accessor")
	TArray<FIntPoint> GetAvailableOptions() const;

	virtual TArray<FIntPoint> GetAvailableOptions_Implementation() const { return TArray<FIntPoint>(); }
};

/**
 * A wrapper for a already defined engine or other settings. (Eg. UGameUserSettings)
 *
 */
UCLASS(Abstract)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapper : public UEnhancedSettingBase
{
	GENERATED_BODY()

	// Get the accessor for this setting
public:
	UFUNCTION()
	virtual UEnhancedSettingWrapperAccessorBase* GetAccessor() const { return nullptr; }
};

/**
 * Bool wrapper for a already defined engine or other settings. (Eg. UGameUserSettings)
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperBool : public UEnhancedSettingWrapper
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Accessor")
	TObjectPtr<UEnhancedSettingWrapperBoolAccessor> Accessor;
	
public:
	virtual UEnhancedSettingWrapperAccessorBase* GetAccessor() const override { return Accessor; }
};

/**
 * Int wrapper for a already defined engine or other settings. (Eg. UGameUserSettings)
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperInt : public UEnhancedSettingWrapper
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Accessor")
	TObjectPtr<UEnhancedSettingWrapperIntAccessor> Accessor;

public:
	virtual UEnhancedSettingWrapperAccessorBase* GetAccessor() const override { return Accessor; }
};

/**
 * Float wrapper for a already defined engine or other settings. (Eg. UGameUserSettings)
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperFloat : public UEnhancedSettingWrapper
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Accessor")
	TObjectPtr<UEnhancedSettingWrapperFloatAccessor> Accessor;
	
public:
	virtual UEnhancedSettingWrapperAccessorBase* GetAccessor() const override { return Accessor; }
};

/**
 * Int2D wrapper for a already defined engine or other settings. (Eg. UGameUserSettings)
 */
UCLASS(Blueprintable, BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSettingWrapperInt2D : public UEnhancedSettingWrapper
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Accessor")
	TObjectPtr<UEnhancedSettingWrapperInt2DAccessor> Accessor;
	
public:
	virtual UEnhancedSettingWrapperAccessorBase* GetAccessor() const override { return Accessor; }
};


/**
 * An asset that stores the settings definition. These are for custom settings
 *
 * It also handles auto UI generation and data validation.
 * The systems that manage these settings handle Save/Load, Reset, Clear changes and Apply.
 */
UCLASS(BlueprintType)
class ENHANCEDSETTINGS_API UEnhancedSetting : public UEnhancedSettingBase
{
	GENERATED_BODY()
 
	virtual FPrimaryAssetId GetPrimaryAssetId() const override{ return FPrimaryAssetId(SettingPrimaryAssetType, GetFName()); }
	static const FPrimaryAssetType SettingPrimaryAssetType;
	
protected:
	// The uniquely identifying 'Dev' tag of the setting, can be used for accessing settings in C++ if needed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Description")
	FGameplayTag UniqueSettingID;
	
	// The scope of the settings, where what subsystem controls them and in what context they will be available for use
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting")
	TEnumAsByte<EEnhancedSettingScope> Scope = GameInstance;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
	// Utility
public:
	virtual EEnhancedSettingScope GetEnhancedSettingScope() const override { return Scope; }
	
	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	virtual FGameplayTag GetUniqueSettingID() const { return UniqueSettingID; }

	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	virtual FText GetSettingName() const { return SettingName; }

	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	virtual FText GetSettingDescription() const { return SettingDescription; }

	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	virtual EEnhancedSettingValueType GetSettingValueType() const { return EEnhancedSettingValueType::NONE; }
};

/**
 * Enhanced Boolean Setting.
 */
UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingBool : public UEnhancedSetting
{
	GENERATED_BODY()

protected:
	virtual EEnhancedSettingValueType GetSettingValueType() const override { return EEnhancedSettingValueType::Boolean; }

	// Boolean
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool Value = true;
	
public:
	UFUNCTION(BlueprintPure, Category = "EnhancedSetting|Value")
	bool GetDefaultValue() const { return Value; }
};

/**
 * Enhanced Integer Setting.
 */
UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingInt : public UEnhancedSetting
{
	GENERATED_BODY()

protected:
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "EnhancedSettingValueType == EEnhancedSettingValueType::Integer", EditConditionHides), DisplayName="Value")
	int Value = 1;
	
	// Delta for slider
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bUseDeltaForSlider = false;
	
	// Delta for slider
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "bUseDeltaForSlider", EditConditionHides, ClampMin = "1", UIMin = "1"))
	int Delta = 1;
	
	// Should we use a scalar UI entry for this integer, with a delta for the slider of 1? If this is false it will use an option list
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bUseSlider = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	TObjectPtr<UEnhancedSettingOptionFilterInt> OptionFilter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	TMap<int, FText> Options;
	
	// Should the value be clamped to the minimum value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bHasMinValue = false;

	// The minimum value the integer can be
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "bHasMinValue", EditConditionHides))
	int MinValue = 0;

	// Should the value be clamped to the maximum value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bHasMaxValue = false;

	// The maximum value the integer can be
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "bHasMaxValue", EditConditionHides))
	int MaxValue = 10;
	
public:
	virtual EEnhancedSettingValueType GetSettingValueType() const override { return EEnhancedSettingValueType::Integer; }
	
	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	int GetDefaultValue() const { return Value; }
};

/**
 * Enhanced Float Setting.
 */
UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingFloat : public UEnhancedSetting
{
	GENERATED_BODY()

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	float Value = 1;
	
	// Delta for slider
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bUseDeltaForSlider = false;
	
	// Delta for slider
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "bUseDeltaForSlider", EditConditionHides, ClampMin = "0.001", UIMin = "0.001"))
	float Delta = 0.01f;
	
	// Logarithmic scale? (e.g., for volume, mouse sensitivity, etc.), This is to allow for more precision in the lower range of values and less fall off in the higher range
	// (IE it converts a multiplicative scale to an linear intuitive scale, meaning the change in value represents a linear change in perception)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bIsLogarithmic = false;
	
	// Should the value be clamped to the minimum value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bHasMinValue = false;

	// The minimum value the float can be
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "bHasMinValue", EditConditionHides))
	float MinValue = 0;

	// Should the value be clamped to the maximum value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	bool bHasMaxValue = false;

	// The maximum value the float can be
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "bHasMaxValue", EditConditionHides))
	float MaxValue = 1;

public:
	virtual EEnhancedSettingValueType GetSettingValueType() const override { return EEnhancedSettingValueType::Float; }
	
	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	float GetDefaultValue() const { return Value; }
};

/**
 * Enhanced Color Setting.
 */
UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingColor : public UEnhancedSetting
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value")
	FColor Value = FColor::White;
	
public:
	virtual EEnhancedSettingValueType GetSettingValueType() const override { return EEnhancedSettingValueType::Color; }

	FColor GetDefaultValue() const { return Value; }
};

/**
 * Enhanced Tag Setting.
 */
UCLASS()
class ENHANCEDSETTINGS_API UEnhancedSettingTag : public UEnhancedSetting
{
	GENERATED_BODY()
	
	/*
	// @TODO - Add support for parent tags, also integrate this with the editor in 2 ways:
	// 1. Popup for selecting tags only derived from the parent tag
	// 2. Auto populate a TMap with the children tags and a localized name (User facing name)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "EnhancedSettingValueType == EEnhancedSettingValueType::Tag", EditConditionHides))
	FGameplayTag SupportedParentTag;

	@Todo: Support for specific tags - explicit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting", meta=(EditCondition = "EnhancedSettingValueType == ESettingValueType::Tag"))
	FGameplayTagContainer SupportedTags;

	@Todo: Support for specific tags - implicit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting", meta=(EditCondition = "EnhancedSettingValueType == ESettingValueType::Tag"))
	FGameplayTag SupportedParentTag;
	
	// When changing the Supported tags ensure the current tag fits into the category or trigger a warning.
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	*/
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "EnhancedSettingValueType == EEnhancedSettingValueType::Tag", EditConditionHides))
	FGameplayTag Value;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "EnhancedSettingValueType == EEnhancedSettingValueType::Tag", EditConditionHides), DisplayName="OptionFilter")
	TObjectPtr<UEnhancedSettingOptionFilterTag> OptionFilter;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedSetting|Value", meta=(EditCondition = "EnhancedSettingValueType == EEnhancedSettingValueType::Tag", EditConditionHides), DisplayName="Options")
	TMap<FGameplayTag, FText> Options;
	
public:
	virtual EEnhancedSettingValueType GetSettingValueType() const override { return EEnhancedSettingValueType::Tag; }
	
	UFUNCTION(BlueprintPure, Category = "EnhancedSetting")
	FGameplayTag GetDefaultValue() const { return Value; }
};


USTRUCT(BlueprintType)
struct FEnhancedSettingInstance
{
	GENERATED_BODY()
	
	TWeakObjectPtr<UEnhancedSetting> Setting;
	FEnhancedSettingValue Value;

	FEnhancedSettingInstance() : Setting(nullptr), Value() {}
	FEnhancedSettingInstance(UEnhancedSetting* InSetting, const FEnhancedSettingValue& InValue) : Setting(InSetting), Value(InValue) {}
};

// A math helper function class
UCLASS()
class UEnhancedSettingsMathFunctionLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "EnhancedSettings|Math")
	static float GetPercentForLogarithmicValue(float Value, float MinValue, float MaxValue)
	{
		if (Value <= 0.0f || MinValue <= 0.0f || MaxValue <= 0.0f || MinValue == MaxValue)
		{
			return 0.0f;
		}

		// Use log10 instead of ln
		const float LogMin = FMath::LogX(10, MinValue);  // log base 10 of MinValue
		const float LogMax = FMath::LogX(10, MaxValue);  // log base 10 of MaxValue
		const float LogValue = FMath::LogX(10, Value);   // log base 10 of Value

		return FMath::GetRangePct(LogMin, LogMax, LogValue);
	}

	UFUNCTION(BlueprintCallable, Category = "EnhancedSettings|Math")
	static float GetLogarithmicValueForPercent(float Percent, float MinValue, float MaxValue)
	{
		if (MinValue <= 0.0f || MaxValue <= 0.0f || MinValue == MaxValue)
		{
			return 0.0f;
		}

		// Clamp Percent to valid range [0, 1]
		Percent = FMath::Clamp(Percent, 0.0f, 1.0f);

		// Use log10 instead of ln
		const float LogMin = FMath::LogX(10, MinValue);  // log base 10 of MinValue
		const float LogMax = FMath::LogX(10, MaxValue);  // log base 10 of MaxValue

		// Interpolate between log10 values
		return FMath::Exp(FMath::Lerp(LogMin, LogMax, Percent));  // Convert back to original scale
	}
};
