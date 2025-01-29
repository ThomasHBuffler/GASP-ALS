// Fill out your copyright notice in the Description page of Project Settings.


#include "FlecsSystems.h"

#include "NiagaraComponent.h"
//#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealFlecs/HealthComponent.h"
#include "UnrealFlecs/Components/FlecsExampleComponents.h"
#include "UnrealFlecs/Subsystems/FlecsGameInstanceSubsystem.h"

inline bool PerformRaycastSingle(UWorld* World, AActor* Owner, float Radius, FVector Location, FVector DesiredDestination, FHitResult& OutHit)
{
	//return FMath::RandBool();
	
	// return UKismetSystemLibrary::SphereTraceSingle(World, Location, DesiredDestination, Radius, TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, false);

	FCollisionQueryParams Params("TraceTag", SCENE_QUERY_STAT_ONLY(KismetTraceUtils), false);
	Params.AddIgnoredActor(Owner);
	Params.bReturnPhysicalMaterial = true;
	
	return FPhysicsInterface::GeomSweepSingle(World, FCollisionShape::MakeSphere(Radius),
		FQuat::Identity,	OutHit, Location, DesiredDestination,
		ECC_Camera, Params, FCollisionResponseParams::DefaultResponseParam,
		FCollisionObjectQueryParams::DefaultObjectQueryParam
	);
}

/*
inline bool PerformRaycastSingleComponent(UWorld* World, AActor* Owner, float Radius, FVector Location, FVector DesiredDestination, FHitResult& OutHit)
{
	//UKismetSystemLibrary::Single
	
	FCollisionQueryParams Params("TraceTag", SCENE_QUERY_STAT_ONLY(KismetTraceUtils), false);
	Params.AddIgnoredActor(Owner);
	Params.bReturnPhysicalMaterial = true;
	
	return FPhysicsInterface::GeomSweepSingle(World, FCollisionShape::MakeSphere(Radius),
		FQuat::Identity,	OutHit, Location, DesiredDestination,
		ECC_Camera, Params, FCollisionResponseParams::DefaultResponseParam,
		FCollisionObjectQueryParams::DefaultObjectQueryParam
	);
	
}*/

inline bool TryToApplyDamage(AActor* InActor, float Damage, float& AppliedDamage)
{
	std::mutex Mutex;	
	if (!IsValid(InActor)) return false;
	UHealthComponent* HealthComp = InActor->FindComponentByClass<UHealthComponent>();
	if (IsValid(HealthComp))
	{
		return HealthComp->ApplyDamage(Damage, AppliedDamage);
	}
	return false;
}

inline void SpawnImpactFX(UWorld* World, flecs::entity Entity, FHitResult Hit)
{
	if (Entity.has<FFlecsProjectileImpactFX>())
	{
		FFlecsProjectileImpactFX ImpactFX = *Entity.get<FFlecsProjectileImpactFX>();
		UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();

		// Check if PhysMat is in the map
		if (ImpactFX.Value.Contains(PhysMat))
		{
			UNiagaraSystem* found = *ImpactFX.Value.Find(PhysMat);
			if (!IsValid(found)) return;
						
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(World,
				found,
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation());
		}
		else
		{
			return;
		}
	}
}

inline bool CanRicochet(FFlecsRicochet Ricochet, FVector Velocity, FVector Normal)
{
	return Ricochet.Angle > FMath::Max((abs(FMath::Acos(FVector::DotProduct(Velocity.GetSafeNormal(), Normal)) * 180 / PI) - 90), 0);
}

inline bool AttemptRicochet(UWorld* World, flecs::entity Entity, FFlecsInstigator* Inst, float Radius, FFlecsLocation* Loc, FFlecsDeltaLocation* Delta, FFlecsVelocity* Vel, FFlecsProjectileInfo* ProjectileImpact)
{
	SpawnImpactFX(World, Entity, ProjectileImpact->LastHitResult);
	FVector TargetDestination = FVector::Zero();
	bool bDidRicochet = false;
	if (Entity.has<FFlecsRicochet>())
	{
		FFlecsRicochet Ricochet = *Entity.get<FFlecsRicochet>();
		if (CanRicochet(Ricochet, Vel->Value, ProjectileImpact->LastHitResult.Normal))
		{
			bDidRicochet = true;
			bool bDoneRicochet = false;
						
			while (!bDoneRicochet)
			{
				FVector HitLocation = ProjectileImpact->LastHitResult.Location + ProjectileImpact->LastHitResult.Normal * .01f;
				FVector NewDelta = FMath::GetReflectionVector(Delta->Value, ProjectileImpact->LastHitResult.Normal)* Ricochet.Restitution;
				TargetDestination = HitLocation + NewDelta;
				FVector NewVelocity = FMath::GetReflectionVector(Vel->Value, ProjectileImpact->LastHitResult.Normal) * Ricochet.Restitution;

				if (NewVelocity.SizeSquared() > Ricochet.MinVelocitySquared)
				{
					Entity.set<FFlecsVelocity>({NewVelocity});
					if (!PerformRaycastSingle(World, Inst->Value, Radius, HitLocation, TargetDestination, ProjectileImpact->LastHitResult))
					{
						bDoneRicochet = true;
						Loc->Value = TargetDestination;
					}
					else
					{
						SpawnImpactFX(World, Entity, ProjectileImpact->LastHitResult);
						if (CanRicochet(Ricochet, Vel->Value, ProjectileImpact->LastHitResult.Normal))
						{
							Delta->Value = NewDelta;
						}
					}
				}
				else
				{
					Entity.add<FFlecsMarkedForDelete>();
					bDoneRicochet = true;
				}
			}
		}
		else
		{
			Entity.add<FFlecsMarkedForDelete>();
		}
	}
	else
	{
		Entity.add<FFlecsMarkedForDelete>();
	}
	return bDidRicochet;
}

inline bool AttemptPenetration(UWorld* World, flecs::entity Entity, FFlecsInstigator* Inst, FVector Velocity, float Radius, FFlecsLocation* Loc, FFlecsVelocity* Vel, FFlecsProjectileInfo* ProjectileImpact)
{
	FVector TargetDestination = FVector::Zero();

	if (Entity.has<FFlecsPenetration>())
	{
		FFlecsPenetration Penetration = *Entity.get<FFlecsPenetration>();
		UPhysicalMaterial* PhysMat = ProjectileImpact->LastHitResult.PhysMaterial.Get();

		if (!Penetration.PenetrationMap.Contains(PhysMat)) return false;
		FFlecsPenetrationEntry PenetrationValues = *Penetration.PenetrationMap.Find(PhysMat);

		FHitResult HitResult = FHitResult();
		FVector HitLocation;
		FVector HitNormal;
		FName HitName;
		
		//DrawDebugSphere(World, ProjectileImpact->LastHitResult.Location, Radius, 12, FColor::Green, false, 2, 0, 1);
		if (ProjectileImpact->LastHitResult.Component.Get()->K2_SphereTraceComponent(Loc->Value + Velocity.GetSafeNormal() * (PenetrationValues.MaxDistance + Radius * 2), Loc->Value, Radius, false, false, true, HitLocation, HitNormal, HitName, HitResult))
		{
			if (!HitResult.bStartPenetrating)
			{
				SpawnImpactFX(World, Entity, HitResult);
				TargetDestination = HitResult.Location + HitResult.Normal * 0.01;
				Loc->Value = TargetDestination;
				Vel->Value = Velocity * PenetrationValues.SpeedMultiplier;
				return true;
			}
		}
		return false;
	}
	return false;
}

/* Create timer entity
inline void CreateTimerEntityFromClass(flecs::world* FlecsWorld, flecs::entity Entity, UTimedEvent* TimerClass)
{
	flecs::entity Proj = FlecsWorld->entity()
	.set<FFlecsTimer>({Entity, TimerClass->TimeToActivate, TimerClass->EventComponents });
}
*/

void UFlecsSystems::InitializeProjectileSystems(UWorld* World, flecs::world* FlecsWorld, UFlecsGameInstanceSubsystem* FlecsWorldSubsytem)
{
	checkf(World, TEXT("World is null"));
	// Set thread count
	FlecsWorld->set_threads(2);
	
#pragma region Projectile Phases
	// Create two custom phases that branch off of EcsOnUpdate. Note that the
	// phases have the Phase tag, which is necessary for the builtin pipeline
	// to discover which systems it should run.
	auto PhysicsPhaseForceAcceleration = FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(flecs::OnUpdate);
	
	auto PhysicsPhaseClampSpeed = FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(PhysicsPhaseForceAcceleration);
		
	auto ComputeDeltaLocationPhaseInitial = FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(PhysicsPhaseClampSpeed);
	
	auto ComputeDeltaLocationPhaseAdditive = FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(ComputeDeltaLocationPhaseInitial);
	
	auto SweepMovement = FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(ComputeDeltaLocationPhaseAdditive);

	auto HitProcessor = FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(SweepMovement);
	
	auto UpdateWorldRepresentation= FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(HitProcessor);
		
	auto Cleanup = FlecsWorld->entity()
	.add(flecs::Phase)
	.depends_on(UpdateWorldRepresentation);
#pragma endregion

#pragma region SolveForces
	// Update velocity with gravity
	auto SGravityForce = FlecsWorld->system<FFlecsVelocity, FFlecsGravity>("Gravity System")
		.kind(PhysicsPhaseForceAcceleration)
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsVelocity* Vel, FFlecsGravity* Grav)
		{
			for (int i : it)
			{
				Vel[i].Value.Z += Grav[i].Value * it.delta_time();
			}
		});

	// Update velocity with local acceleration
	auto SLocalAcceleration = FlecsWorld->system<FFlecsVelocity, FFlecsLocalAcceleration>("Local Acceleration System")
		.kind(PhysicsPhaseForceAcceleration)
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsVelocity* Vel, FFlecsLocalAcceleration* LocAccel)
		{
			for (int i : it)
			{
				Vel[i].Value += (Vel[i].Value.GetSafeNormal() * LocAccel[i].Value * it.delta_time());
			}
		});

	// Clamp the velocity to the max speed
	auto SClampVelocity = FlecsWorld->system<FFlecsVelocity, FFlecsTerminalVelocity>("Clamp Velocity")
		.kind(PhysicsPhaseClampSpeed)
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsVelocity* Vel, FFlecsTerminalVelocity* TermVel)
		{
			for (int i : it)
			{
				if (Vel[i].Value.SizeSquared() > TermVel[i].Value.TerminalVelocitySqaured)
				{
					Vel[i].Value = UKismetMathLibrary::ClampVectorSize(Vel[i].Value, 0, TermVel[i].Value.TerminalVelocity);
				}
			}
		});
#pragma endregion
	
#pragma region Update Delta Location
	// Set the Delta Location using the velocity component
	auto SUpdateTargetLocation =
		FlecsWorld->system<FFlecsDeltaLocation, FFlecsVelocity>("Update Projectile Delta Location")
		.kind(ComputeDeltaLocationPhaseInitial)
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsDeltaLocation* Deltaloc, FFlecsVelocity* Vel)
		{
			for (int i : it)
			{
				Deltaloc[i].Value = Vel[i].Value * it.delta_time();
			}
		});

	// Set the Delta Location using the gravity acceleration component, D += 0.5 * A * t^2
	auto SUpdateProjectileDeltaLocationGravity =
		FlecsWorld->system<FFlecsDeltaLocation, FFlecsGravity>("Update Projectile Delta Location - Gravity")
		.kind(ComputeDeltaLocationPhaseAdditive)
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsDeltaLocation* Deltaloc, FFlecsGravity* Grav)
		{
			for (int i : it)
			{
				Deltaloc[i].Value += 0.5 * FVector(0, 0, Grav[i].Value) * (it.delta_time() * it.delta_time());
			}
		});

	// Set the Delta Location using the local acceleration component, D += 0.5 * A * t^2
	auto SUpdateProjectileDeltaLocationLocalAcceleration =
		FlecsWorld->system<FFlecsDeltaLocation, FFlecsVelocity, FFlecsLocalAcceleration>("Update Projectile Delta Location - Local Acceleration")
		.kind(ComputeDeltaLocationPhaseAdditive)
		.multi_threaded()
		.iter([](flecs::iter it, FFlecsDeltaLocation* Deltaloc, FFlecsVelocity* Vel, FFlecsLocalAcceleration* LocAccel)
		{
			for (int i : it)
			{
				Deltaloc[i].Value += 0.5 * (Vel[i].Value.GetSafeNormal() * LocAccel[i].Value) * (it.delta_time() * it.delta_time());
			}
		});
#pragma endregion

#pragma region SweepMovement
	auto SRaycastMovement = FlecsWorld->system<
	FFlecsInstigator, FFlecsProjectileRadius, FFlecsLocation, FFlecsDeltaLocation, FFlecsProjectileInfo>("Sweep Movement System")
	.kind(SweepMovement)
	.multi_threaded()
	.without<FFlecsMarkedForDelete>()
	.iter([FlecsWorldSubsytem](flecs::iter it, FFlecsInstigator* Inst, FFlecsProjectileRadius* Radi, FFlecsLocation* Loc, FFlecsDeltaLocation* Delta, FFlecsProjectileInfo* ProjectileImpact)
	{
		for (int i : it)
		{
			FVector TargetDestination = Loc[i].Value + Delta[i].Value;	
	
			if (!PerformRaycastSingle(FlecsWorldSubsytem->GetWorld(), Inst[i].Value, Radi[i].Value, Loc[i].Value, TargetDestination, ProjectileImpact[i].LastHitResult))
			{
				Loc[i].Value = TargetDestination;
			}
			else
			{
				it.entity(i).add<FFlecsHitNeedsProcessing>();
			}
		}
		
	});

	auto SHitProcessor = FlecsWorld->system<
	FFlecsInstigator, FFlecsProjectileRadius, FFlecsHitNeedsProcessing, FFlecsLocation, FFlecsVelocity, FFlecsDeltaLocation, FFlecsProjectileInfo>("Hit ReactionSystem System")
	.kind(HitProcessor)
	//.multi_threaded()
	.without<FFlecsMarkedForDelete>()
	.iter([FlecsWorldSubsytem](flecs::iter it, FFlecsInstigator* Inst, FFlecsProjectileRadius* Radi, FFlecsHitNeedsProcessing* Hit,FFlecsLocation* Loc, FFlecsVelocity* Vel, FFlecsDeltaLocation* Delta, FFlecsProjectileInfo* ProjectileImpact)
	{
		for (int i : it)
		{
			FVector TargetDestination = Loc[i].Value + Delta[i].Value;	
			bool bChangeOccured = true;
			if (!AttemptPenetration(FlecsWorldSubsytem->GetWorld(), it.entity(i), Inst, Vel[i].Value, Radi[i].Value, Loc, Vel, ProjectileImpact))
			{
				if(!AttemptRicochet(FlecsWorldSubsytem->GetWorld(), it.entity(i), Inst, Radi[i].Value, Loc, Delta, Vel, ProjectileImpact))
				{
					bChangeOccured = false;
				}
			}
			if(!bChangeOccured) Loc[i].Value = TargetDestination;
				
			SpawnImpactFX(FlecsWorldSubsytem->GetWorld(), it.entity(i), ProjectileImpact[i].LastHitResult);
			it.entity(i).set<FFlecsProjectileInfo>({FHitResult(), });
			it.entity(i).remove<FFlecsHitNeedsProcessing>();
		}
	});
	
#pragma endregion
	
#pragma region UpdateWorldRepresentation
	auto SUpdateNiagaray = FlecsWorld->system<FFlecsProjectileNiagaraComponent, FFlecsLocation, FFlecsVisualOffset, FFlecsVelocity>("Update WorldRepresentation")
	.kind(UpdateWorldRepresentation)
	//.multi_threaded()
	.without<FFlecsMarkedForDelete>()
	.iter([](flecs::iter it, FFlecsProjectileNiagaraComponent* NiagComp, FFlecsLocation* Loc, FFlecsVisualOffset* VisOff, FFlecsVelocity* Vel)
	{
		for (int i : it)
		{
			if (NiagComp[i].Value)
			{
					NiagComp[i].Value->SetWorldLocation(Loc[i].Value + VisOff[i].Value);
					VisOff[i].Value = FMath::VInterpTo(VisOff[i].Value, FVector::Zero(), it.delta_time(), 1);
					NiagComp[i].Value->SetWorldRotation(Vel[i].Value.Rotation());
			}
		}
	});
#pragma endregion
	
#pragma region Cleanup
	auto SCleanup = FlecsWorld->system<FFlecsMarkedForDelete, FFlecsProjectile>("Cleanup Entities")
	.kind(Cleanup)
	//.multi_threaded()
	.iter([](flecs::iter it, FFlecsMarkedForDelete* Del, FFlecsProjectile* Proj)
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
						NiagComp->SetBoolParameter("Alive", false);
						NiagComp->ReleaseToPool();
					}
				}
				entity.destruct();
			}
		}
	});
#pragma endregion 
}


/*
 *
 *

#pragma region SweepMovement
	auto SRaycastMovement = FlecsWorld->system<
	FFlecsInstigator, FFlecsProjectileRadius, FFlecsLocation, FFlecsVelocity, FFlecsDeltaLocation, FFlecsProjectileInfo>("Sweep Movement System")
	.kind(SweepMovement)
	.without<FFlecsMarkedForDelete, FFlecsRicochet>()
	.iter([GameInstance->World](flecs::iter it, FFlecsInstigator* Inst, FFlecsProjectileRadius* Radi, FFlecsLocation* Loc, FFlecsVelocity* Vel, FFlecsDeltaLocation* Delta, FFlecsProjectileInfo* ProjectileImpact)
	{
		for (int i : it)
		{
			FVector TargetDestination = Loc[i].Value + Delta[i].Value;	
	
			if (PerformRaycastSingle(World, Inst[i].Value, Radi[i].Value, Loc[i].Value, TargetDestination, ProjectileImpact[i].LastHitResult))
			{
				SpawnImpactFX(World, it.entity(i), ProjectileImpact[i].LastHitResult);

				
				if (it.entity(i).has<FFlecsRicochet>())
				{
					FFlecsRicochet Ricochet = *it.entity(i).get<FFlecsRicochet>();
					FVector HitLocation = ProjectileImpact[i].LastHitResult.Location + ProjectileImpact[i].LastHitResult.Normal;
					FVector NewDelta = FMath::GetReflectionVector(Delta[i].Value, ProjectileImpact[i].LastHitResult.Normal)* Ricochet.Restitution;
					FVector TargetLocation = HitLocation + NewDelta;
					
					FVector NewVelocity = FMath::GetReflectionVector(Vel[i].Value, ProjectileImpact[i].LastHitResult.Normal) * Ricochet.Restitution;

					if (NewVelocity.Size() > Ricochet.MinVelocity)
					{
						it.entity(i).set<FFlecsVelocity>({NewVelocity});
						Loc[i].Value = TargetLocation;
					}
					else
					{
						it.entity(i).add<FFlecsMarkedForDelete>();
					}
					
					/*
					// DrawDebugDirectionalArrow(World, ProjectileImpact[i].LastHitResult.Location, TargetLocation, 100, FColor::Red, false, 2, 0, 1);

					// Print the delta size, green for 10 seconds
					// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Delta Size: %f"), Delta[i].Value.Size()));
						
					// Draw debug point on hit location
					// DrawDebugPoint(World, HitLocation, 10, FColor::Red, false, 2, 0);
					
						
				
				}
				else
				{
					it.entity(i).add<FFlecsMarkedForDelete>();
				}
			}
			else
			{
				Loc[i].Value = TargetDestination;
			}
		}
	});

 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
*if (bHit)
{
if (it.entity(i).has<FFlecsRicochet>())
{
FFlecsRicochet Ricochet = *it.entity(i).get<FFlecsRicochet>();
Ricochet.Count --;
					
it.entity(i).set<FFlecsRicochet>({Ricochet});
FVector CurrentLocation = ProjectileImpact[i].LastHitResult.Location;
bool bExit = false;
while (bHit || bExit)
{
// Setup values
float DistRemaining =
Delta[i].Value.Size() - (ProjectileImpact[i].LastHitResult.ImpactPoint - CurrentLocation).Size();

FVector NewDirection = FMath::GetReflectionVector(Delta[i].Value.GetSafeNormal(), ProjectileImpact[i].LastHitResult.ImpactNormal);

FVector NewDelta = NewDirection * DistRemaining * Ricochet.Restitution;
Delta[i].Value = NewDelta;
						
FVector StartLocation = ProjectileImpact[i].LastHitResult.Location + ProjectileImpact[i].LastHitResult.Normal * 0.05f;
FVector TargetLocation = StartLocation + NewDelta;
						
Vel[i].Value = NewDirection * Ricochet.Restitution;
}
}
else
{
SpawnImpactFX(World, it.entity(i), ProjectileImpact[i].LastHitResult);
it.entity(i).add<FFlecsMarkedForDelete>();
}
}*/