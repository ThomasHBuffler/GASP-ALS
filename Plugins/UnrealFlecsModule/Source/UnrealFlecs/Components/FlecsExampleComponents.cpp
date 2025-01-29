// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsExampleComponents.h"

/*

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealFlecs/Flecs/flecs.h"

//
 // General explination of flecs workflow:
 // firstly flecs Prefabs will store shared and default values for components.
 // Prefabs are entities themselves and so you need to be careful to not delete / modify them unintentionally.
 //
 // We will store the prefabs in the GameInstanceSubsystem so they are shared between levels and so that when we tick
 // the FlecsWorldSubsystem, they would not be modified by world systems. (EXAMPLE, Lifetime gets depeleted and then adds MarkForDelete, then the entity is deleted)
 //
 // We need to solve the following problems:
 // Firstly where are prefabs defined, remember we need to be able to define them in an editor friendly way. (DataAsset?)
 // Secondly, we'd need to save the prefabs to a file so that they can be loaded when the game starts. (DataAsset, OnLoad?)
 // Thirdly, we'd need to be able to spawn prefabs from the FlecsGameInstanceSubsystem in the FlecsWorldSubsystem.
 //

#pragma region Editor
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UNREALFLECS_API UFlecsEditorFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void AddComponentToEntity(flecs::entity Entity) const {}
};
#pragma endregion

#define DEFINE_FLECS_COMPONENT(ComponentName, ComponentType, SuperType) \
struct F##ComponentName \
{ \
ComponentType Value; \
}; \
ECS_COMPONENT_DECLARE(ComponentName); \
UCLASS() \
class UNREALFLECS_API UF##ComponentName##Component : public SuperType \
{ \
GENERATED_BODY() \
public: \
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) \
ComponentType Value; \
	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.set<F##ComponentName>({ Value });
	}
\

// These are basic example components to illustrate flecs working with Unreal Engine
// I am still trying to understand ecs design so these are very basic and are very likely not be the best way to do things.



#pragma region LifeTime
struct FFlecsLifetime
{
	float Value;
};

ECS_COMPONENT_DECLARE(Lifetime);

UCLASS(Blueprintable, meta = (DisplayName = "Flecs Lifetime Component"))
class UFlecsLifetimeComponent : public UFlecsEditorFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Lifetime;

	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.set<FFlecsLifetime>({ Lifetime });
	}
};
#pragma endregion

#pragma region float
DEFINE_FLECS_COMPONENT(FloatingPointValue, float, UFlecsEditorFragment)
#pragma endregion


struct FFlecsMarkForDelete {};

// MarkForDelete - this is a tag component,
// realistically no prefab should have this component, Then again... what about a timer that deletes the entity after...
// FTimer would handle invoking adding another component to the entity that would perform alternate actions.

// How would I abstractualize this in an efficient manner?

struct FFlecsTimer
{
	float LifeTime;
	// Now we need a pointer to the component that will be added to the entity when the timer is up.
};


*/

/*

/////////////////////////////////////////////////////////////////////    DAMAGE SYSTEM    /////////////////////////////////////////////////////////////////////

// Define what types of impacts are avaiable, Self, Neutral, Team, Enemy.
// All damageables will have a health component, and a way to calculate damage affiliation.
// If its not a damageable we assume its the environment. (Well we treat the collision as if it where the environment)
UENUM(BlueprintType)
enum class EDamageableAffiliation : uint8
{
	Environment,
	Self,
	Neutral,	
	Team,
	Enemy,
};

// Define a ImpactEffectMagnitudeModifier, this is a way to modify the effect magnitude based on certain conditions
#pragma region ImpactEffectMagnitudeModifier
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class UImpactEffectMagnitudeModifier: public UObject
{
	GENERATED_BODY()
public:
	// This is called before the impact response is applied, and can be used to modify the damage value
	virtual void ModifyImpactResponse(flecs::entity Entity, float& EffectMagnitude) {}
};

UCLASS()
class UImpactMagnitudeModifierDistance : public UImpactEffectMagnitudeModifier
{
	GENERATED_BODY()
public:
    // Curve that modifies the effect magnitude based on distance
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* DistanceModifierCurve;
	
	virtual void ModifyImpactResponse(flecs::entity ProjectileEntity, float& EffectMagnitude)
	{
	    float DistanceTraveled = ProjectileEntity.get<FFlecsProjectileInfo>()->TraveledDistance;
	    EffectMagnitude = DistanceModifierCurve->GetFloatValue(DistanceTraveled);
	}
};

UCLASS()
class UImpactMagnitudeModifierCharge : public UImpactEffectMagnitudeModifier
{
	GENERATED_BODY()
public:
	// Curve that modifies the effect magnitude based on charge
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* ChargeModifierCurve;
	
	virtual void ModifyImpactResponse(flecs::entity ProjectileEntity, float& EffectMagnitude)
	{
		
	    //float Charge = Entity.get<FFlecsProjectileInfo>()->Charge;
	    //EffectMagnitude = ChargeModifierCurve->GetFloatValue(Charge);
	    
	}
};
#pragma endregion 

#pragma region Damage/Healing/StatusEffect (AKA Effect)
USTRUCT(BlueprintType)
struct FProjectileGameplayEffectValue
{
	GENERATED_BODY()
	
	// The tag of the effect (Eg, Damage, Healing, Sheild, etc)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag EffectTag;

	// The magnitude of the effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Magnitude;

};

USTRUCT(BlueprintType)
struct FProjectileGameplayEffectDirect
{
	GENERATED_BODY()

	// Effect to target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FProjectileGameplayEffectValue Effect;
	
	// Stack based effects, eg. Poison, Burn, Freeze.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, int> StackEffects;

	// A list of magnitude modifiers that can modify the effect magnitude, Order matters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<UImpactEffectMagnitudeModifier*> MagnitudeModifiers;
	
	virtual FProjectileGameplayEffectValue CalculateAppliedEffects(flecs::entity ProjectileEntity)
	{
		FProjectileGameplayEffectValue AppliedEffect = Effect;
		for (UImpactEffectMagnitudeModifier* MagnitudeModifier : MagnitudeModifiers)
		{
			MagnitudeModifier->ModifyImpactResponse(ProjectileEntity, AppliedEffect.Magnitude);
		}
		return AppliedEffect;
	}
		
	virtual TMap<FGameplayTag, int> CalculateStackedEffects(flecs::entity ProjectileEntity)
	{
		return StackEffects;
	}
};

USTRUCT(BlueprintType)
struct FProjectileGameplayEffectRadial
{
	GENERATED_BODY()

	// Effect to target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FProjectileGameplayEffectValue Effect;
	
	// Stack based effects, eg. Poison, Burn, Freeze.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FGameplayTag, int> StackEffects;

	// A list of magnitude modifiers that can modify the effect magnitude, Order matters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<UImpactEffectMagnitudeModifier*> MagnitudeModifiers;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* DistanceModifierCurve;

	virtual FProjectileGameplayEffectValue GetAppliedEffect(flecs::entity ProjectileEntity, float NormalizedDistance)
	{
		FProjectileGameplayEffectValue AppliedEffect = Effect;
		AppliedEffect.Magnitude = Effect.Magnitude * DistanceModifierCurve->GetFloatValue(NormalizedDistance);
		for (UImpactEffectMagnitudeModifier* MagnitudeModifier : MagnitudeModifiers)
		{
			MagnitudeModifier->ModifyImpactResponse(ProjectileEntity, AppliedEffect.Magnitude);
		}
		return AppliedEffect;
	}
		
	virtual TMap<FGameplayTag, int> CalculateStackedEffects(flecs::entity ProjectileEntity)
	{
		return StackEffects;
	}
};
#pragma endregion

#pragma region ImpactResponse
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class UImpactResponseFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual bool bAttemptToApplyImpactResponse(flecs::entity ProjectileEntity, TArray<AActor*>& IgnoredActors)
	{
		return false;
	}
};

UCLASS()
class UImpactResponseDirectDamageFragment : public UImpactResponseFragment
{
	GENERATED_BODY()
public:
	// Effect to apply on impact, these are direct effects, such as damage, healing, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EDamageableAffiliation, FProjectileGameplayEffectDirect> DirectEffect;
	
	
	// Non affliated gameplay action
	// We can do many things here, set a timer to mark for delete and bounce, explode, Modify projectiles charge,
	// stick to surface, Mark for delete, etc.
	// @TODO IMPLEMENT THIS
	//
	
	virtual bool bAttemptToApplyImpactResponse(flecs::entity ProjectileEntity, TArray<AActor*>& IgnoredActors)
	{
		AActor* HitActor = ProjectileEntity.get<FFlecsProjectileInfo>()->LastHitResult.GetActor();
		// EDamageableAffiliation HitActor->GetAffiliationComponent()->GetAffiliation(ProjectileEntity.get<FFlecsInstigator>());
		// if (!Responses.Contains(Affiliation)) return false;
		
		// FStatsComponent StatsComponent = HitActor->GetStatsComponent();
		// bool bWasApplied = StatsComponent.ApplyEffect(Responses[Affiliation]);
		// return bWasApplied;
		return false;	
	};	
};


UCLASS()
class UImpactResponseDirectExplode : public UImpactResponseFragment
{
	GENERATED_BODY()
public:
	// Effect to apply on impact, these are direct effects, such as damage, healing, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EDamageableAffiliation, FProjectileGameplayEffectRadial> DirectEffect;
	
	// Non affliated gameplay action
	// We can do many things here, set a timer to mark for delete and bounce, explode, Modify projectiles charge,
	// stick to surface, Mark for delete, etc.
	// @TODO IMPLEMENT THIS
	//
	
	virtual bool bAttemptToApplyImpactEffectResponse(flecs::entity ProjectileEntity, TArray<AActor*>& IgnoredActors)
	{
		AActor* HitActor = ProjectileEntity.get<FFlecsProjectileInfo>()->LastHitResult.GetActor();
		// EDamageableAffiliation HitActor->GetAffiliationComponent()->GetAffiliation(ProjectileEntity.get<FFlecsInstigator>());
		// if (!Responses.Contains(Affiliation)) return false;
		
		// FStatsComponent StatsComponent = HitActor->GetStatsComponent();
		// bool bWasApplied = StatsComponent.ApplyEffect(Responses[Affiliation]);
		// return bWasApplied;
		return false;	
	};

	virtual void ApplyImpactReactionResponse(flecs::entity ProjectileEntity)
	{
		// Bounce, 
	};
	
};


UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class UImpactManagerFragment : public UFlecsEditorFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	UImpactResponseFragment* ImpactResponse;

};
*/