// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealFlecs/Flecs/flecs.h"
#include "FlecsSystems.generated.h"


/**
 * 
 */
UCLASS()
class UNREALFLECS_API UFlecsSystems : public UObject
{
	GENERATED_BODY()
	
public:
	// Initialize systems
	static void InitializeSystems(UWorld* World, flecs::world* FlecsWorld, class UFlecsGameInstanceSubsystem* FlecsWorldSubsytem)
	{
		UFlecsSystems::InitializeProjectileSystems(World, FlecsWorld, FlecsWorldSubsytem);
	}
	
protected:
	static void InitializeProjectileSystems(UWorld* World, flecs::world* FlecsWorld, UFlecsGameInstanceSubsystem* FlecsWorldSubsytem);
	
};

/* OLD
 
#prgama region SolveForces
	// Clamp the velocity to the max speed
	auto SClampVelocity = FlecsWorld->system<FFlecsVelocity, FFlecsTerminalVelocity>("Clamp Velocity")
	.multi_threaded()
	.iter([](flecs::iter it, FFlecsVelocity* Vel, FFlecsTerminalVelocity* TermVel)
	{
		for (int i : it)
		{
			if (Vel[i].Value.Size() > TermVel[i].Value)
			{
				Vel[i].Value = Vel[i].Value.GetSafeNormal() * TermVel[i].Value;
			}
		}
	});

#pragma endregion

#pragma region UpdateDeltaLocation
	// Set the Delta Location, velocity component
	auto SUpdateTargetLocation =
		FlecsWorld->system<FFlecsDeltaLocation,  FFlecsVelocity>("Update Projectile Delta Location")
	.multi_threaded()
	.iter([World](flecs::iter it, FFlecsDeltaLocation* Deltaloc, FFlecsVelocity* Vel)
	{
		for (int i : it)
		{
			Deltaloc[i].Value = Vel[i].Value * it.delta_time();
		}
	});

	// Set the Delta Location, gravity acceleration component, D += 0.5 * A * t^2
	auto SUpdateProjectileDeltaLocationGravity =
		FlecsWorld->system<FFlecsDeltaLocation, FFlecsGravity>("Update Projectile Delta Location - Gravity")
	.multi_threaded()
	.iter([World](flecs::iter it, FFlecsDeltaLocation* Deltaloc, FFlecsGravity* Grav)
	{
		for (int i : it)
		{
			Deltaloc[i].Value += 0.5 * FVector(0, 0, Grav[i].Value) * (it.delta_time() * it.delta_time());
		}
	});

	// Set the Delta Location, local acceleration component, D += 0.5 * A * t^2
	auto SUpdateProjectileDeltaLocationLocalAcceleration =
		FlecsWorld->system<FFlecsDeltaLocation, FFlecsVelocity, FFlecsLocalAcceleration>("Update Projectile Delta Location - Local Acceleration")
	.multi_threaded()
	.iter([World](flecs::iter it, FFlecsDeltaLocation* Deltaloc, FFlecsVelocity* Vel, FFlecsLocalAcceleration* LocAccel)
	{
		for (int i : it)
		{
			Deltaloc[i].Value += 0.5 * (Vel[i].Value.GetSafeNormal() * LocAccel[i].Value) * (it.delta_time() * it.delta_time());
		}
	});

	// Assume the Local Jerk component has a velocity component, and so part of the equation ( d = v*t ) the remaining is 1/6 * j * t^3
	auto SUpdateProjectileDeltaLocationLocalJerk =
		FlecsWorld->system<FFlecsDeltaLocation, FFlecsVelocity, FFlecsLocalJerk>("Update Projectile Delta Location - Local Jerk")
	.multi_threaded()
	.iter([World](flecs::iter it, FFlecsDeltaLocation* Deltaloc, FFlecsVelocity* Vel, FFlecsLocalJerk* LocJerk)
	{
		for (int i : it)
		{
			Deltaloc[i].Value += (0.166667) * (Vel[i].Value.GetSafeNormal() * LocJerk[i].Value) * (it.delta_time() * it.delta_time() * it.delta_time());
		}
	});
#pragma endregion

#pragma region UpdateVelocity
	// Gravity Force v=v+at
	auto SGravityForce = FlecsWorld->system<FFlecsVelocity, FFlecsGravity>("Gravity System")
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsVelocity* Vel, FFlecsGravity* Grav)
		{
			for (int i : it)
			{
				Vel[i].Value.Z += Grav[i].Value * it.delta_time();
			}
		});

	// Local Acceleration Force  v=v+at
	auto SLocalAcceleration = FlecsWorld->system<FFlecsVelocity, FFlecsLocalAcceleration>("Local Acceleration System")
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsVelocity* Vel, FFlecsLocalAcceleration* LocAccel)
		{
			for (int i : it)
			{
				Vel[i].Value += (Vel[i].Value.GetSafeNormal() * LocAccel[i].Value * it.delta_time());
			}
		});

	// Local Jerk Force 
	auto SLocalJerk = FlecsWorld->system<FFlecsLocalJerk, FFlecsVelocity, FFlecsLocalAcceleration>("Local Jerk System")
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsLocalJerk* LocJerk, FFlecsVelocity* Vel, FFlecsLocalAcceleration* LocAccel)
		{
			for (int i : it)
			{
				// Local Jerk force is applied to the world velocity v=v+0.5jt^2
				Vel[i].Value += (0.5 * (Vel[i].Value.GetSafeNormal() * LocJerk[i].Value)) * (it.delta_time() * it.delta_time());
				
				// Local Jerk force is applied to the Local acceleration a=a+jt
				LocAccel[i].Value += LocJerk[i].Value * it.delta_time();
			}
		});


#pragma endregion

	
	// Sweep
	auto SRaycastMovement = FlecsWorld->system<
	FFlecsInstigator, FFlecsProjectileRadius, FFlecsLocation, FFlecsDeltaLocation, FFlecsProjectileInfo>("Sweep Movement System")
	.without<FFlecsMarkedForDelete>()
	.iter([World](flecs::iter it, FFlecsInstigator* Inst, FFlecsProjectileRadius* Rad, FFlecsLocation* Loc, FFlecsDeltaLocation* Delta, FFlecsProjectileInfo* ProjectileImpact)
	{
		for (int i : it)
		{
			FVector TargetDestination = Loc[i].Value + Delta[i].Value;
			if (PerformRaycastSingle(World, Inst[i].Value, Rad[i].Value, Loc[i].Value, TargetDestination, ProjectileImpact[i].LastHitResult))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("" + ProjectileImpact[i].LastHitResult.GetActor()->GetName()));
				if (it.entity(i).has<FFlecsProjectileImpactFX>())
				{
					FFlecsProjectileImpactFX ImpactFX = *it.entity(i).get<FFlecsProjectileImpactFX>();
					UPhysicalMaterial* PhysMat = ProjectileImpact[i].LastHitResult.PhysMaterial.Get();

					// Check if PhysMat is in the map
					auto found = ImpactFX.Value.Find(PhysMat);
					if (*found != nullptr)
					{
						UNiagaraSystem* ImpactSystem = *found;
						
							UNiagaraFunctionLibrary::SpawnSystemAtLocation(World,
								ImpactSystem,
								ProjectileImpact[i].LastHitResult.ImpactPoint,
								ProjectileImpact[i].LastHitResult.ImpactNormal.Rotation());
					}
					else
					{
						// Handle the error: PhysMat was not found in the map
						// This could be a log message, an exception, etc., depending on your error handling strategy
					}
				}
				it.entity(i).add<FFlecsMarkedForDelete>();
				
			}
			else
			{
				// Allow other systems to handle the hit, literally just set the hit result, don't even adjust the position (This will allow the other systems to use delta time, once, for this update)
				Loc[i].Value = TargetDestination;
			}
		}
	});


	auto SCleanup = FlecsWorld->system<FFlecsMarkedForDelete>("Cleanup Entities")
	//.multi_threaded()
	.iter([](flecs::iter it, FFlecsMarkedForDelete* Del)
	{
		for (int i : it)
		{
			auto entity = it.entity(i);
			if (entity.is_alive())
			{
				if (entity.has<FFlecsProjectileNiagaraComponent>())
				{
					UNiagaraComponent* NiagComp = entity.get<FFlecsProjectileNiagaraComponent>()->Value;
					if (IsValid(NiagComp))
					{
						NiagComp->DestroyComponent();
					}
				}
				entity.destruct();
			}
		}
	});
	
}
*/