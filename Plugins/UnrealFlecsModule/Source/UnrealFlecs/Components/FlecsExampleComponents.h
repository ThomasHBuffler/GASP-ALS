// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnrealFlecs/Flecs/flecs.h"
#include "FlecsExampleComponents.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

// @TODO I need to figure out how I can implicitly declare dependencies for components
// For example, an FlecsProjectile component will ALWAYS have a FlecsLocation component, FlecsProjectileInfo, FlecsImpactFX, etc
// I need to figure out how to make this implicit, so that I don't have to manually add these components to the entity
// One way to do this is to have a single editor facing component that has all the other components as properties.
// Its a good solution for implicit problems such as the projectile example above, but it might not be the best solution
// for more generalized components, such as the Location component, which can be used in many different contexts

#define DEFINE_FLECS_COMPONENT(ComponentName, ComponentType) \
struct FFlecs##ComponentName \
{ \
ComponentType Value; \
}; \
//ECS_COMPONENT_DECLARE(ComponentName); \

#define DEFINE_FLECS_COMPONENT_2(ComponentName, ComponentType1, TypeName1, ComponentType2, TypeName2) \
struct FFlecs##ComponentName \
{ \
ComponentType1 TypeName1; \
ComponentType2 TypeName2; \
}; \

#define DEFINE_FLECS_COMPONENT_3(ComponentName, ComponentType1, TypeName1, ComponentType2, TypeName2, ComponentType3, TypeName3) \
struct FFlecs##ComponentName \
{ \
ComponentType1 TypeName1; \
ComponentType2 TypeName2; \
ComponentType3 TypeName3; \
}; \

#define DEFINE_FLECS_COMPONENT_4(ComponentName, ComponentType1, TypeName1, ComponentType2, TypeName2, ComponentType3, TypeName3, ComponentType4, TypeName4) \
struct FFlecs##ComponentName \
{ \
ComponentType1 TypeName1; \
ComponentType2 TypeName2; \
ComponentType3 TypeName3; \
ComponentType4 TypeName4; \
}; \

#define DEFINE_FLECS_MAP_COMPONENT(ComponentName, KeyType, ValueType) \
struct FFlecs##ComponentName \
{ \
TMap<KeyType, ValueType> Value; \
}; \

#define DEFINE_FLECS_TAG(ComponentName) \
struct FFlecs##ComponentName {}; \
//ECS_COMPONENT_DECLARE(ComponentName); \

#pragma region EditorFacing
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UNREALFLECS_API UFlecsEditorFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void AddComponentToEntity(flecs::entity Entity) const {}
};
#pragma endregion

// GENERAL COMPONENTS

#pragma region Owner
DEFINE_FLECS_COMPONENT(Instigator, AActor*)
#pragma endregion

#pragma region LifeTime
DEFINE_FLECS_COMPONENT(Lifetime, float)

UCLASS()
class UNREALFLECS_API ULifetimeFragment : public UFlecsEditorFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Lifetime;
	
	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.set_override<FFlecsLifetime>({ Lifetime });
	}
};
#pragma endregion

#pragma region Location
DEFINE_FLECS_COMPONENT(Location, FVector)
UCLASS()
class UNREALFLECS_API ULocationFragment : public UFlecsEditorFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Location;
	
	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.set_override<FFlecsLocation>({ Location });
	}
};
#pragma endregion

#pragma region VisualLocation
DEFINE_FLECS_COMPONENT(VisualOffset, FVector)
#pragma endregion

#pragma region Rotation
DEFINE_FLECS_COMPONENT(Rotation, FRotator)

UCLASS()
class UNREALFLECS_API URotationFragment : public UFlecsEditorFragment
{
	GENERATED_BODY()
public:
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly)
	FRotator Rotation;

	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.set_override<FFlecsRotation>({ Rotation });
	}
};
#pragma endregion

#pragma region Physics
// Velocity
DEFINE_FLECS_COMPONENT(Velocity, FVector)
DEFINE_FLECS_COMPONENT(DeltaLocation, FVector)

UCLASS()
class UNREALFLECS_API UVelocityFragment : public UFlecsEditorFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;
	
	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.set_override<FFlecsVelocity>({ Velocity });
		Entity.set_override<FFlecsDeltaLocation>(FFlecsDeltaLocation());
	}
};

// Gravity
DEFINE_FLECS_COMPONENT(Gravity, int16)
DEFINE_FLECS_COMPONENT(LocalAcceleration, uint16);
DEFINE_FLECS_COMPONENT(LocalJerk, uint16);

UCLASS()
class UNREALFLECS_API UGravityFragment : public UFlecsEditorFragment
{
	GENERATED_BODY()
public:
	// The gravity (0 means no gravity)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Forces", meta = (ClampMin = -32768, ClampMax = 32767, ForceUnits = "cm/s^2"))
	int Gravity;
	
	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.set_override<FFlecsGravity>({ static_cast<int16>(Gravity)});
	}
};
#pragma endregion 

#pragma region MarkForDelete
DEFINE_FLECS_TAG(MarkedForDelete)

UCLASS()
class UMarkForDelete : public UFlecsEditorFragment
{
	GENERATED_BODY()

public:
	virtual void AddComponentToEntity(flecs::entity Entity) const override
	{
		Entity.add<FFlecsMarkedForDelete>();
	}
};
#pragma endregion

// PROJECTILE COMPONENTS

#pragma region Projectile
DEFINE_FLECS_TAG(Projectile)
#pragma endregion

#pragma region Speed

struct FTerminalVelocity
{
	float TerminalVelocity;
	float TerminalVelocitySqaured;
	
};

DEFINE_FLECS_COMPONENT(ProjectileSpeed, uint16_t)

DEFINE_FLECS_COMPONENT(DragFactor, float)

DEFINE_FLECS_COMPONENT(TerminalVelocity, FTerminalVelocity)
#pragma endregion

#pragma region ProjectileRadius
DEFINE_FLECS_COMPONENT(ProjectileRadius, float)
#pragma endregion

#pragma region Ricochet
DEFINE_FLECS_COMPONENT_3(Ricochet, float, Angle, float, Restitution, float, MinVelocitySquared)
#pragma endregion

#pragma region Penetration
USTRUCT(BlueprintType)
struct FFlecsPenetrationEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier;
};

struct FFlecsPenetration 
{ 
	float MinExitVelocitySquared;
	TMap<UPhysicalMaterial*, FFlecsPenetrationEntry> PenetrationMap;
};

DEFINE_FLECS_MAP_COMPONENT(PenetrationMap, UPhysicalMaterial*, float)
#pragma endregion

#pragma region ProjectileImpactFX
DEFINE_FLECS_MAP_COMPONENT(ProjectileImpactFX, UPhysicalMaterial*, UNiagaraSystem*)
#pragma endregion

#pragma region ProjectileLiveFX
DEFINE_FLECS_COMPONENT(ProjectileNiagaraSystem, UNiagaraSystem*)
DEFINE_FLECS_COMPONENT(ProjectileNiagaraComponent, UNiagaraComponent*)
#pragma endregion

#pragma region ProjectileInfo
// This component is used to store gameplay important information about the projectile
// This includes, Traveled Distance, Last Hit Result, etc, @TODO I need to make a lightweight version of this
struct FFlecsProjectileInfo
{
	FHitResult LastHitResult;
	
	//float TraveledDistance;
};

DEFINE_FLECS_TAG(HitNeedsProcessing)
#pragma endregion

#pragma region DamageEffect
// Alright there can be multiple types of ways to apply damage, and responses apon application.
// For example, a projectile can apply damage on impact, or it can apply damage over time.
// It can also apply damage to radial damage, and or direct damage.
// It could also apply knockback, or apply a status effect.

// Projectile specific responses to the damage effect can also be varied.
// For example on impacting a damageable actor and successfuly applying damage,
// It may be destoyed.
// It may bounce off the actor.
// It may spawn more projectiles or other entities.

// Then there is other inputs that could modify this, Such as projectile weapon charge, characters stats, etc.

// To summarize, this is a very complex part of the projectile system, and it needs to be very flexible and modular.
// @TODO IMPLEMENT AND RESEARCH THIS SYSTEM - this is currently a placeholder

struct FFlecsDamageEffect
{
	float Damage;
};
#pragma endregion

#pragma region Projectile
UCLASS()
class UProjectileFragment : public UFlecsEditorFragment
{
	GENERATED_BODY()
	
public:
#pragma region Speed
	// The speed of the projectile (0 means stationary)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Speed") //,  meta = (ClampMin = 0,  ClampMax = 65535, ForceUnits="cm/s"))
	float Speed;
	
	// The max speed of the projectile (0 means no limit)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Speed") //, meta = (ClampMin = 0,  ClampMax = 65535,  ForceUnits="cm/s"))
	float TerminalVelocity;
#pragma endregion

#pragma region Forces
	// The Z (Up/Down) gravity of the projectile (0 means no gravity)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Forces", meta = (ClampMin = -32768, ClampMax = 32767, ForceUnits = "cm/s^2"))
	int Gravity;

	// 'Accelration' force to apply in direction of motion (0 means none)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Forces",  meta = (ClampMin = 0,  ClampMax = 65535,  ForceUnits = "cm/s^2"))
	int LocalAcceleration;
#pragma	endregion
	
	/*
	// 'Jerk' (Change in acceleration) force to apply in direction of motion (0 means none)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Forces",  meta = (ClampMin = 0,  ClampMax = 65535,  ForceUnits = "cm/s^2"))
	int LocalJerk;
	
	// The drag of the projectile (0 means no drag)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Speed")//, meta = (ClampMin = 0.0f))
	float DragFactor;
	*/
	
#pragma region Collision
	// The radius of the projectile (0 means line /point) @TODO investigate stationary projectiles with 0 radius.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Collision", meta = (ClampMin = 0.0f, Units = "cm"))
	float Radius;

	/* Ricochet */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Ricochet")
	bool bRicochets;
	
	// The angle at which the projectile will ricochet (0 means no ricochet, 90 means always ricochet)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Collision|Ricochet", meta = (ClampMin = 0.0f, ClampMax = 90.0f, Units = "Degrees", EditCondition = "bRicochets"))
	float RicochetAngle;

	// The restitution of the ricochet (0 means no bounce, 1 means perfect bounce, over 1 means increased speed)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Collision|Ricochet", meta = (ClampMin = 0.0f, ClampMax = 1.0f, EditCondition = "bRicochets"))
	float RicochetRestitution;

	// The minimum velocity to ricochet (0 or negative means always ricochet (Bounce))
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Collision|Ricochet", meta = (EditCondition = "bRicochets"))
	float MinRicochetVelocity;
	
	/* Penetration */
	// Should the projectile penetrate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Penetratation")
	bool bPenetrates;
	
	// The minimum exit velocity to penetrate (0 means no penetration)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Penetratation",meta = (EditCondition = "bPenetrates"))
	float MinPenetrationExitVelocity;
	
	// The mapping of physical materials to penetration speed multipliers
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Penetratation", meta = (EditCondition = "bPenetrates"))
	TMap<UPhysicalMaterial*, FFlecsPenetrationEntry> PenetrationMap;
#pragma endregion
	
	//  The projectiles live visual and audio world representation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* NiagaraSystem;
	
	// The Damage to apply on impact
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage;
	
	// The mapping of physical materials to impact FX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<UPhysicalMaterial*, UNiagaraSystem*> ImpactFXMap;
	
	virtual void AddComponentToEntity(flecs::entity Entity) const
	{
		//  Projectile Prefab defined components (Editor defined)
		Entity.add<FFlecsProjectile>();
		Entity.set<FFlecsDamageEffect>({ Damage });
		Entity.set_override<FFlecsInstigator>({nullptr});
		
		if (NiagaraSystem)
		{
			Entity.set<FFlecsProjectileNiagaraSystem>({ NiagaraSystem });
			Entity.set_override<FFlecsProjectileNiagaraComponent>({ nullptr });
		}
		if (Speed > 0)
		{
			Entity.set<FFlecsProjectileSpeed>({static_cast<uint16_t>(Speed)});

			Entity.set_override<FFlecsVelocity>({});
			Entity.set_override<FFlecsDeltaLocation>({});
		}
		
		/* // DRAG
		if (DragFactor > 0)
		{
			Entity.set<FFlecsDragFactor>({DragFactor});
		}*/
		
		if (TerminalVelocity > 0)
		{
			FTerminalVelocity TermVel;
			TermVel.TerminalVelocity = static_cast<uint16_t>(TerminalVelocity);
			TermVel.TerminalVelocitySqaured = TermVel.TerminalVelocity * TermVel.TerminalVelocity;
			Entity.set<FFlecsTerminalVelocity>({TermVel});
		}
		if (Gravity!= 0)
		{
			Entity.set<FFlecsGravity>({ static_cast<int16>(Gravity)});
		}
		if (bRicochets && RicochetAngle > 0)
		{
			Entity.set<FFlecsRicochet>({ RicochetAngle, RicochetRestitution, MinRicochetVelocity*MinRicochetVelocity});
		}
		if (bPenetrates)
		{
			Entity.set<FFlecsPenetration>({ MinPenetrationExitVelocity, PenetrationMap });
		}
		
		/* // LOCAL JERK
		if (LocalJerk > 0)
		{
			Entity.set<FFlecsLocalJerk>({static_cast<uint16_t>(LocalJerk)});
			Entity.set<FFlecsLocalAcceleration>({static_cast<uint16_t>( LocalAcceleration > 0 ? LocalAcceleration : 0)});
		}
		else if (LocalAcceleration > 0)
		{
			Entity.set<FFlecsLocalAcceleration>({static_cast<uint16_t>(LocalAcceleration)});
		}
		*/
		
		if (LocalAcceleration > 0)
		{
			Entity.set<FFlecsLocalAcceleration>({static_cast<uint16_t>(LocalAcceleration)});
		}
		Entity.set<FFlecsProjectileRadius>({ Radius });
		Entity.set<FFlecsProjectileImpactFX>({ ImpactFXMap });
		Entity.set_override<FFlecsProjectileInfo>({});
	}
};
#pragma endregion

// EVENT COMPONENTS @TODO NEEDS WORK
/*
struct FFlecsTimer
{
	// Entity that the timed event should grant the components to
	flecs::entity Entity;

	float TimeRemaining;
	
	TArray<UFlecsEditorFragment*> FragmentsToAddToEntity;

	TArray<UTimedEvent*> TimedEventsToCreate;
};

#pragma region TimedEvent
UCLASS(DefaultToInstanced, EditInlineNew)
class UTimedEvent : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeToActivate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<UFlecsEditorFragment*> EventComponents;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<UTimedEvent*> TimedEvents;
	
	virtual void CreateTimedEventEntity(flecs::world* World, flecs::entity OwnerEntity) const
	{
		flecs::entity TimedEventEntity = World->entity();
	}
};
#pragma endregion
*/